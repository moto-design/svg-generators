/*
 *  moto-design SGV utils.
 */

#define _GNU_SOURCE
#define _ISOC99_SOURCE

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <assert.h>
#include <errno.h>
#include <fenv.h>
#include <stdlib.h>
#include <string.h>

#include "geometry.h"
#include "log.h"
#include "mem.h"
#include "svg.h"

FILE *debug_svg_stream;

struct point_c *polar_to_cart(const struct point_p *p, struct point_c *c)
{
	int fe_err;
	float rad = deg_to_rad(p->t);

	errno = 0;
	feclearexcept(FE_ALL_EXCEPT);

	c->x = p->r * cosf(rad);
	fe_err = fetestexcept(FE_INVALID);

	c->y = p->r * sinf(rad);
	fe_err |= fetestexcept(FE_INVALID);

	if (errno || fe_err) {
		error("Math error: %d (%s)\n", fe_err, strerror(errno));
		assert(0);
		exit(EXIT_FAILURE);
	}

	return c;
}

struct point_p *cart_to_polar(const struct point_c *c, struct point_p *p)
{
	int fe_err;

	errno = 0;
	feclearexcept(FE_ALL_EXCEPT);

	p->r = sqrtf(c->x * c->x + c->y * c->y);
	fe_err = fetestexcept(FE_INVALID);

	p->t = deg_to_rad(atan2f(c->y, c->x));
	fe_err |= fetestexcept(FE_INVALID);

	if (errno || fe_err) {
		error("Math error: %d (%s)\n", fe_err, strerror(errno));
		assert(0);
		exit(EXIT_FAILURE);
	}

	return p;
}

void debug_print_cart(const char *msg, const struct point_c *c)
{
	debug("%scart:   x = %f, y = %f\n", msg, c->x, c->y);
}

void debug_print_polar(const char *msg, const struct point_p *p)
{
	debug("%spolar: r = %f, t = %f\n", msg, p->r, p->t);
}

void debug_print_pc(const char *msg, const struct point_pc *pc)
{
	debug_print_polar(msg, &pc->p);
	debug_print_cart(msg, &pc->c);
}

void debug_print_line(const char *msg, const struct line_c *line)
{
	debug("%sa         = {%f, %f}\n", msg, line->a.x, line->a.y);
	debug("%sb         = {%f, %f}\n", msg, line->b.x, line->b.y);
	debug("%sslope     = %f deg\n", msg, rad_to_deg(line->slope));
	debug("%sintercept = %f\n", msg, line->intercept);

	if (debug_svg_stream) {
		struct svg_line sl;

		sl.a = line->a;
		sl.b = line->b;
		svg_write_line(debug_svg_stream, &svg_style_red_red, msg, &sl);
	}
}

struct point_c line_intersection(const struct line_c *line1,
	const struct line_c *line2)

{
	static const float reject_limit = 0.1;
	const float s_diff = line1->slope - line2->slope;
	struct point_c i;

	debug("slope diff = %f\n", s_diff);

	if (s_diff > -reject_limit && s_diff < reject_limit) {
		error("No intersection (%f).\n", s_diff);
		debug_print_line("line1 ", line1);
		debug_print_line("line2 ", line2);
		assert(0);
		exit(EXIT_FAILURE);
	}

	if (!isfinite(line1->slope)) {
		i.x = line1->a.x;
		i.y = line2->slope * i.x + line2->intercept;
	} else if (!isfinite(line2->slope)) {
		i.x = line2->a.x;
		i.y = line1->slope * i.x + line1->intercept;
	} else {
		i.x = (line2->intercept - line1->intercept) / (s_diff);
		i.y = line1->slope * i.x + line1->intercept;
	}

	debug_print_cart("intersection ", &i);
	return i;
}

void polygon_star_init(const struct star_params *star_params,
	struct polygon_star *ps)
{
	struct line_c seg1;
	struct line_c seg2;
	struct point_pc pc;
	struct point_p p;

	ps->points = star_params->points;
	ps->radius = star_params->radius;
	ps->rotation = star_params->rotation;
	ps->sector_angle = 360.0 / (2.0 * star_params->points);

	p.r = star_params->radius;

	p.t = 0.0;
	polar_to_cart(&p, &seg1.a);

	p.t += 2.0 * ps->sector_angle * star_params->density;
	polar_to_cart(&p, &seg1.b);

	seg1.slope = line_slope(&seg1);
	seg1.intercept = line_intercept(&seg1);

	debug_print_line("seg1 ", &seg1);

	p.t = 2.0 * ps->sector_angle;
	polar_to_cart(&p, &seg2.a);

	p.t -= 2.0 * ps->sector_angle * star_params->density;
	polar_to_cart(&p, &seg2.b);

	seg2.slope = line_slope(&seg2);
	seg2.intercept = line_intercept(&seg2);

	debug_print_line("seg2 ", &seg2);

	pc.c = line_intersection(&seg1, &seg2);
	pc_cart_to_polar(&pc);

	ps->inner_radius = pc.p.r;

	debug("points       = %u\n", star_params->points);
	debug("density      = %u\n", star_params->density);
	debug("radius       = %f\n", star_params->radius);
	debug("rotation     = %f\n", star_params->rotation);
	debug("sector_angle = %f\n", ps->sector_angle);
	debug("inner_radius = %f\n", ps->inner_radius);
}

void polygon_star_generate(const struct polygon_star *ps,
	struct node_buffer *nb)
{
	struct point_p p;
	unsigned int node;

	nb->node_count = 2.0 * ps->points;
	nb->nodes = mem_alloc(nb->node_count * sizeof(*nb->nodes));

	for (node = 0, p.r = ps->radius, p.t = ps->rotation;
		node < nb->node_count;
		node++, p.t += ps->sector_angle,
		p.r = (node % 2) ? ps->inner_radius : ps->radius) {
		polar_to_cart(&p, &nb->nodes[node]);

		debug("node_%u: polar = {%f, %f},\tcart = {%f, %f}\n",
			node, p.t, p.r, nb->nodes[node].x, nb->nodes[node].y);
	}
}

void node_buffer_clean(struct node_buffer *nb)
{
	mem_free(nb->nodes);
}
