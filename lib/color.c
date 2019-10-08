/*
 *  moto-design SGV utils.
 */

#define _GNU_SOURCE
#define _ISOC99_SOURCE

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"
#include "log.h"
#include "mem.h"
#include "util.h"

bool is_hex_color(const char *str)
{
	assert(str);

	return (str[0] == '#'
		&& str[1] && isxdigit(str[1])
		&& str[2] && isxdigit(str[2])
		&& str[3] && isxdigit(str[3])
		&& str[4] && isxdigit(str[4])
		&& str[5] && isxdigit(str[5])
		&& str[6] && isxdigit(str[6])
		&& str[7] == 0);
}

void hex_color_set(char *color, const char *value)
{
	assert(color);
	assert(value);

	if (!value || !is_hex_color(value)) {
		fprintf(stderr, "Bad hex color value: '%s'\n", value);
		assert(0);
		exit(EXIT_FAILURE);
	}

	debug("value = '%s'\n", value);

	color[0] = value[0];
	color[1] = value[1];
	color[2] = value[2];
	color[3] = value[3];
	color[4] = value[4];
	color[5] = value[5];
	color[6] = value[6];
	color[7] = 0;
}

void palette_fill(struct palette *palette, const struct color_data *data,
	unsigned int data_len)
{
	unsigned int i;
	unsigned int out;

	if (palette->colors) {
		mem_free(palette->colors);
		palette->colors = NULL;
	}

	for (i = 0, palette->color_count = 0; i < data_len; i++) {
		palette->color_count += data[i].weight;
	}

	palette->colors =  mem_alloc(palette->color_count * hex_color_len);

	for (i = 0, out = 0; i < data_len; i++) {
		unsigned int j;
		for (j = 0; j < data[i].weight; j++, out++) {
			debug("Add %s\n", data[i].value);
			memcpy(&palette->colors[out], data[i].value,
				hex_color_len);
		}
	}
}

const char *palette_get_random(const struct palette *palette)
{
	return palette->colors[random_unsigned(0, palette->color_count - 1)];
}
