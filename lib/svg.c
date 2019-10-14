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

struct svg_fill *svg_fill_set(struct svg_fill *fill, const char *color)
{
	debug("color = '%s'\n", color);

	hex_color_set(fill->color, color);
	return fill;
}

struct svg_stroke *svg_stroke_set(struct svg_stroke *stroke, const char *color,
	unsigned int width)
{
	debug("color = '%s', width = %u\n", color, width);

	stroke->width = width;
	hex_color_set(stroke->color, color);
	return stroke;
}

void svg_open_svg(FILE *stream, const struct svg_rect *background_rect)
{
	fprintf(stream, "<svg \n"
		"  xmlns=\"http://www.w3.org/2000/svg\"\n"
		"  xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"\n");

	if (background_rect) {
		fprintf(stream, 
//			"  width=\"%f\"\n"
//			"  height=\"%f\"\n"
			"  viewBox=\"%f %f %f %f\">\n",
//			background_rect->width, background_rect->width,
			background_rect->x, background_rect->y,
			background_rect->width, background_rect->height);
	}
}

void svg_close_svg(FILE *stream)
{
	fprintf(stream, "</svg>\n");
}

void svg_open_group(FILE *stream, const char *id)
{
	fprintf(stream,
		" <g  id=\"%s\" inkscape:label=\"%s\" inkscape:groupmode=\"layer\">\n",
		id, id);
}

void svg_close_group(FILE *stream)
{
	fprintf(stream, " </g>\n");
}

void svg_open_object(FILE *stream, const char *type, const char *id,
	const struct svg_style *style)
{
	fprintf(stream, "  <%s id=\"%s\"\n", type, id);

	if (style && is_hex_color(style->fill.color)) {
		fprintf(stream, "   fill=\"%s\"\n", style->fill.color);
	}

	if (style && is_hex_color(style->stroke.color)) {
		fprintf(stream, "   stroke=\"%s\" stroke-width=\"%u\"\n",
			style->stroke.color, style->stroke.width);
	}
}

void svg_close_object(FILE *stream)
{
	fprintf(stream, "  />\n");
}

void svg_open_path(FILE *stream, const char *id, const struct svg_style *style)
{
	svg_open_object(stream, "path", id, style);
}

void svg_open_polygon(FILE *stream, const char *id,
	const struct svg_style *style)
{
	svg_open_object(stream, "polygon", id, style);
	fprintf(stream, "   transform=\"translate(0,0)\"\n");
	fprintf(stream, "   points=\"\n");
}

void svg_close_polygon(FILE *stream)
{
	fprintf(stream, "   \"\n");
}

void svg_write_line(FILE *stream, const char *id, const struct svg_line *line,
	const struct svg_style *style)
{
	svg_open_object(stream, "line", id, style);
//x1="0" y1="0" x2="200" y2="200"
	fprintf(stream,
		"   x1=\"%f\" y1=\"%f\"\n  x2=\"%f\" y2=\"%f\"\n",
		line->a.x, line->a.y, line->b.x, line->b.y);

	svg_close_object(stream);
}

void svg_write_rect(FILE *stream, const char *id, const struct svg_rect *rect,
	const struct svg_style *style)
{
	svg_open_object(stream, "rect", id, style);

	fprintf(stream,
		"   width=\"%f\"\n   height=\"%f\"\n   x=\"%f\"\n   y=\"%f\"\n   rx=\"%f\"\n",
		rect->width, rect->height, rect->x, rect->y, rect->rx);

	svg_close_object(stream);
}

void svg_write_background(FILE* out_stream, const struct svg_style *style,
	const struct svg_rect *background_rect)
{
	assert(is_hex_color(style->fill.color));

	svg_open_group(out_stream, "background");
	svg_write_rect(out_stream, "background", background_rect, style);
	svg_close_group(out_stream);
}
