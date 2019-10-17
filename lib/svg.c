/*
 *  moto-design SGV utils.
 */

#define _GNU_SOURCE
#define _ISOC99_SOURCE

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <assert.h>

#include "log.h"
#include "svg.h"

static FILE *debug_stream;

FILE *svg_debug_stream()
{
	return debug_stream;
}

void svg_debug_stream_set(FILE *stream)
{
	debug_stream = stream;
}

struct svg_fill *svg_fill_set(struct svg_fill *fill, const char *color)
{
	assert(fill);
	//debug("color = '%s'\n", color);

	if (color) {
		hex_color_set(fill->color, color);
	} else {
		hex_color_clear(fill->color);
	}
	return fill;
}

struct svg_stroke *svg_stroke_set(struct svg_stroke *stroke, const char *color,
	unsigned int width)
{
	assert(stroke);
	//debug("color = '%s', width = %u\n", color, width);

	if (color && width) {
		stroke->width = width;
		hex_color_set(stroke->color, color);
	} else {
		stroke->width = 0;
		hex_color_clear(stroke->color);
	}
	return stroke;
}

void svg_open_svg(FILE *stream, const struct svg_rect *background_rect)
{
	fprintf(stream, "<svg \n"
		"  xmlns=\"http://www.w3.org/2000/svg\"\n"
		"  xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"");

	if (background_rect) {
		fprintf(stream, "\n"
//			"  width=\"%f\"\n"
//			"  height=\"%f\"\n"
			"  viewBox=\"%f %f %f %f\"",
//			background_rect->width, background_rect->width,
			background_rect->x, background_rect->y,
			background_rect->width, background_rect->height);
	}
	fprintf(stream, ">\n");
}

void svg_close_svg(FILE *stream)
{
	fprintf(stream, "</svg>\n");
}

static void svg_write_style(FILE *stream, const struct svg_style *style)
{
	TODO.
	if (style) {
		if (is_hex_color(style->fill.color)) {
			fprintf(stream, "   fill=\"%s\"\n", style->fill.color);
		}

		if (is_hex_color(style->stroke.color)) {
			fprintf(stream, "   stroke=\"%s\" stroke-width=\"%u\"\n",
				style->stroke.color, style->stroke.width);
		}
	}
}

static void svg_write_transform(FILE *stream,
	const struct svg_transform *transform)
{
	if (transform) {
		fprintf(stream, "   transform=\"\n");

		if (transform->translate.x != null_point_c.x) {
			fprintf(stream, "     translate(%f %f)\n",
				transform->translate.x, transform->translate.y);
		}

		if (transform->scale.x != null_point_c.x) {
			fprintf(stream, "     scale(%f %f)\n",
				transform->scale.x, transform->scale.y);
		}

		if (transform->rotation.angle) {
			fprintf(stream, "     rotate(%f",
				transform->rotation.angle);
			if (transform->rotation.p.x != null_point_c.x) {
				fprintf(stream, "%f %f",
					transform->rotation.p.x,
					transform->rotation.p.y);
			}
			fprintf(stream, ")\n");
		}

		fprintf(stream, "   \"\n");
	}
}

void svg_open_group(FILE *stream, const struct svg_style *style,
	const struct svg_transform *transform, const char *id)
{
	fprintf(stream,
		" <g  id=\"%s\" inkscape:label=\"%s\" inkscape:groupmode=\"layer\"",
		id, id);

	svg_write_style(stream, style);
	svg_write_transform(stream, transform);

	fprintf(stream, ">\n");
}

void svg_close_group(FILE *stream)
{
	fprintf(stream, " </g>\n");
}

void svg_open_object(FILE *stream, const struct svg_style *style,
	const struct svg_transform *transform, const char *id, const char *type)
{
	fprintf(stream, "  <%s id=\"%s\"\n", type, id);

	svg_write_style(stream, style);
	svg_write_transform(stream, transform);
}

void svg_close_object(FILE *stream)
{
	fprintf(stream, "  />\n");
}

void svg_open_path(FILE *stream, const struct svg_style *style,
	const struct svg_transform *transform, const char *id)
{
	svg_open_object(stream, style, transform, id, "path");
}

void svg_open_polygon(FILE *stream, const struct svg_style *style,
	const struct svg_transform *transform, const char *id)
{
	svg_open_object(stream, style, transform, id, "polygon");
	fprintf(stream, "   points=\"\n");
}

void svg_close_polygon(FILE *stream)
{
	fprintf(stream, "   \"\n");
	svg_close_object(stream);
}

void svg_write_line(FILE *stream, const struct svg_style *style,
	const struct svg_transform *transform, const char *id,
	const struct svg_line *line)
{
	svg_open_object(stream, style, transform, id, "line");
//x1="0" y1="0" x2="200" y2="200"
	fprintf(stream,
		"   x1=\"%f\" y1=\"%f\"\n  x2=\"%f\" y2=\"%f\"\n",
		line->a.x, line->a.y, line->b.x, line->b.y);

	svg_close_object(stream);
}

void svg_write_rect(FILE *stream, const struct svg_style *style,
	const struct svg_transform *transform, const char *id,
	const struct svg_rect *rect)
{
	svg_open_object(stream, style, transform, id, "rect");

	fprintf(stream,
		"   width=\"%f\"\n   height=\"%f\"\n   x=\"%f\"\n   y=\"%f\"\n   rx=\"%f\"\n",
		rect->width, rect->height, rect->x, rect->y, rect->rx);

	svg_close_object(stream);
}

void svg_write_background(FILE *stream, const struct svg_style *style,
	const struct svg_transform *transform,
	const struct svg_rect *background_rect)
{
	assert(is_hex_color(style->fill.color));

	svg_open_group(stream, style, transform, "background");
	svg_write_rect(stream, style, transform, "background", background_rect);
	svg_close_group(stream);
}

void svg_write_star(FILE *stream, const struct svg_style *style,
	const struct svg_transform *transform, const char *id,
	const struct star_params *star_params)
{
	struct node_buffer nb;
	unsigned int node;

	polygon_star_setup(star_params, &nb);
	svg_open_polygon(stream, style, transform, id);

	for (node = 0; node < nb.node_count; node++) {

		fprintf(stream, "     %f,%f\n", nb.nodes[node].x, nb.nodes[node].y);
		//debug("node_%u: cart = {%f, %f}\n", node, nb.nodes[node].x,
		//	nb.nodes[node].y);
	}

	svg_close_polygon(stream);
	node_buffer_clean(&nb);
}
