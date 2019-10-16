/*
 *  moto-design SGV utils.
 */

#if ! defined(_MD_GENERATOR_SVG_H)
#define _MD_GENERATOR_SVG_H

#include <stdio.h>

#include "color.h"
#include "geometry.h"

struct svg_fill {
	char color[hex_color_len];
};

struct svg_stroke {
	unsigned int width;
	char color[hex_color_len];
	char *dasharray;
};

struct svg_style {
	struct svg_fill fill;
	struct svg_stroke stroke;
};

struct svg_line {
	struct point_c a;
	struct point_c b;
};

struct svg_rect {
	float width;	// size.
	float height;
	float x;	// position.
	float y;
	float rx;	// corner radius.
	float ry;
};

static const struct svg_style svg_style_royal_no_stroke = {
	.fill.color = _hex_color_royal,
	.stroke.color = _hex_color_null,
	.stroke.width = 0,
	.stroke.dasharray = NULL,
};

static const struct svg_style svg_style_light_gray_no_stroke = {
	.fill.color = _hex_color_light_gray,
	.stroke.color = _hex_color_null,
	.stroke.width = 0,
	.stroke.dasharray = NULL,
};

static const struct svg_style svg_style_gray_no_stroke = {
	.fill.color = _hex_color_gray,
	.stroke.color = _hex_color_null,
	.stroke.width = 0,
	.stroke.dasharray = NULL,
};

static const struct svg_style svg_style_red_red = {
	.fill.color = _hex_color_red,
	.stroke.color = _hex_color_red,
	.stroke.width = 1,
	.stroke.dasharray = NULL,
};

static const struct svg_style svg_style_yellow_blue = {
	.fill.color = _hex_color_yellow,
	.stroke.color = _hex_color_blue,
	.stroke.width = 1,
	.stroke.dasharray = NULL,
};

static const struct svg_style svg_style_light_green_light_green = {
	.fill.color = _hex_color_light_green,
	.stroke.color = _hex_color_light_green,
	.stroke.width = 1,
	.stroke.dasharray = NULL,
};

static const struct svg_style svg_style_black_black = {
	.fill.color = _hex_color_black,
	.stroke.color = _hex_color_black,
	.stroke.width = 1,
	.stroke.dasharray = NULL,
};

static const struct svg_style svg_style_white_white = {
	.fill.color = _hex_color_white,
	.stroke.color = _hex_color_white,
	.stroke.width = 1,
	.stroke.dasharray = NULL,
};

struct svg_fill *svg_fill_set(struct svg_fill *fill, const char *color);
struct svg_stroke *svg_stroke_set(struct svg_stroke *stroke, const char *color,
	unsigned int width);
static inline struct svg_style *svg_style_set(struct svg_style *style, const char *fill_color,
	const char *stroke_color, unsigned int stroke_width)
{
	svg_fill_set(&style->fill, fill_color);
	svg_stroke_set(&style->stroke, stroke_color, stroke_width);
	return style;
}

void svg_open_svg(FILE *stream, const struct svg_rect *background_rect);
void svg_close_svg(FILE *stream);

void svg_open_group(FILE *stream, const char *id);
void svg_close_group(FILE *stream);

void svg_open_object(FILE *stream, const struct svg_style *style,
	const char *id, const char *type);
void svg_close_object(FILE *stream);

void svg_open_path(FILE *stream, const struct svg_style *style, const char *id);

void svg_open_polygon(FILE *stream, const struct svg_style *style,
	const char *id);
void svg_close_polygon(FILE *stream);

void svg_write_line(FILE *stream, const struct svg_style *style, const char *id,
	const struct svg_line *line);
void svg_write_rect(FILE *stream, const struct svg_style *style,
	const char *id, const struct svg_rect *rect);
void svg_write_background(FILE* out_stream, const struct svg_style *style,
	const struct svg_rect *background_rect);
void svg_write_star(FILE* out_stream, const struct svg_style *style,
	const char *id, const struct star_params *star_params);

#endif /* _MD_GENERATOR_SVG_H */
