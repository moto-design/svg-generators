/*
 *  moto-design SGV utils.
 */

#if ! defined(_MD_GENERATOR_COLOR_H)
#define _MD_GENERATOR_COLOR_H

#include <stdbool.h>

#define hex_color_len sizeof("#000000")

#define _hex_color_white       "#ffffff"
#define _hex_color_light_gray  "#eeeeee"
#define _hex_color_silver      "#c0c0c0"
#define _hex_color_gray        "#808080"
#define _hex_color_black       "#000000"

#define _hex_color_blue        "#001aff"
#define _hex_color_true_blue   "#0000ff"
#define _hex_color_royal       "#000099"

#define _hex_color_red         "#ff0000"
#define _hex_color_yellow      "#ffdd00"
#define _hex_color_light_green "#1aff70"


static const char hc_white[hex_color_len]       = _hex_color_white;
static const char hc_light_gray[hex_color_len]  = _hex_color_light_gray;
static const char hc_silver[hex_color_len]      = _hex_color_silver;
static const char hc_gray[hex_color_len]        = _hex_color_gray;
static const char hc_black[hex_color_len]       = _hex_color_black;

static const char hc_blue[hex_color_len]        = _hex_color_blue;
static const char hc_true_blue[hex_color_len]   = _hex_color_true_blue;
static const char hc_royal[hex_color_len]       = _hex_color_royal;

static const char hc_red[hex_color_len]         = _hex_color_red;
static const char hc_yellow[hex_color_len]      = _hex_color_yellow;
static const char hc_light_green[hex_color_len] = _hex_color_light_green;

struct color_data
{
	unsigned int weight;
	char value[hex_color_len];
};

struct palette
{
	unsigned int color_count;
	char (*colors)[hex_color_len];
};

bool is_hex_color(const char *p);
void hex_color_set(char *color, const char *value);

void palette_parse_config(const char *config_file, struct palette *palette);
void palette_fill(struct palette *palette, const struct color_data *data,
	unsigned int data_len);
const char *palette_get_random(const struct palette *palette);

#endif /* _MD_GENERATOR_COLOR_H */
