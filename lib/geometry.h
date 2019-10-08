/*
 *  moto-design SGV utils.
 */

#if ! defined(_MD_GENERATOR_GEOMETRY_H)
#define _MD_GENERATOR_GEOMETRY_H

#include "log.h"

static inline float deg_to_rad(float deg)
{
	return deg * M_PI / 180.0;
}

static inline float rad_to_deg(float rad)
{
	return rad * 180.0 / M_PI;
}

struct point_c {
	float x;
	float y;
};

struct point_p {
	float r;
	float t;
};

struct point_pc {
	struct point_p p;
	struct point_c c;
};

struct point_c *polar_to_cart(const struct point_p *p, struct point_c *c);
struct point_p *cart_to_polar(const struct point_c *c, struct point_p *p);

static inline struct point_c *pc_polar_to_cart(struct point_pc *pc) {
	return polar_to_cart(&pc->p, &pc->c);
};
static inline struct point_p *pc_cart_to_polar(struct point_pc *pc) {
	return cart_to_polar(&pc->c, &pc->p);
};

void debug_print_cart(const char *msg, const struct point_c *c);
void debug_print_polar(const char *msg, const struct point_p *p);
void debug_print_pc(const char *msg, const struct point_pc *pc);

struct line_c {
	struct point_c a;
	struct point_c b;
	float slope;
	float intercept;
};

struct line_p {
	struct point_p a;
	struct point_p b;
};

void debug_print_line(const char *msg, const struct line_c *line);

static inline float line_slope(struct line_c *line)
{	
	return (line->b.y - line->a.y) / (line->b.x - line->a.x);
}

static inline float line_intercept(struct line_c *line)
{
	return line->a.y - (line->slope * line->a.x);
}

static inline struct line_c *line_init(struct line_c *line,
	const struct point_c *a, const struct point_c *b)
{
	line->a = *a;
	line->b = *b;
	line->slope = line_slope(line);
	line->intercept = line_intercept(line);
	
	return line;
}

struct point_c line_intersection(const struct line_c *line1,
	const struct line_c *line2);

struct star_params {
	unsigned int points;
	unsigned int density;
	float radius;
	float rotation;
};

struct polygon_star {
	float points;
	float radius;
	float rotation;
	float sector_angle;
	float inner_radius;
};

struct node_buffer {
	unsigned int node_count;
	struct point_c *nodes;
};

void polygon_star_init(const struct star_params *star_params,
	struct polygon_star *ps);
void polygon_star_generate(const struct polygon_star *ps,
	struct node_buffer *nb);
static inline void polygon_star_setup(const struct star_params *star_params,
	struct node_buffer *nb)
{
	struct polygon_star ps;

	polygon_star_init(star_params, &ps);
	polygon_star_generate(&ps, nb);
}

void node_buffer_clean(struct node_buffer *nb);

#endif /* _MD_GENERATOR_GEOMETRY_H */
