/*
 *  moto-design US flag generator.
 */

/*
  PROJECT="${HOME}/projects/moto-design/svg-generators"
  (cd ${PROJECT} && ./bootstrap) && ${PROJECT}/configure --enable-debug
*/

#define _GNU_SOURCE
#define _ISOC99_SOURCE

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <string.h>
#include <time.h>

#include "svg-utils.h"

static const char program_name[] = "flag-generator";

static void print_version(void)
{
	printf("%s (" PACKAGE_NAME ") " PACKAGE_VERSION "\n", program_name);
}

static void print_bugreport(void)
{
	fprintf(stderr, "Report bugs at " PACKAGE_BUGREPORT ".\n");
}

enum opt_value {opt_undef = 0, opt_yes, opt_no};

struct opts {
	float height;
	char *output_file;
	enum opt_value help;
	enum opt_value verbose;
	enum opt_value version;
};

static void print_usage(const struct opts *opts)
{
	print_version();

	fprintf(stderr,
"%s - Generates SVG file of a flag.\n"
"Usage: %s [flags]\n"
"Option flags:\n"
"  --height          - Height of flag. Default: '%f'.\n"
"  -o --output-file  - Output file. Default: '%s'.\n"
"  -h --help         - Show this help and exit.\n"
"  -v --verbose      - Verbose execution.\n"
"  -V --version      - Display the program version number.\n",
		program_name, program_name,
		opts->height,
		opts->output_file
	);

	print_bugreport();
}

static int opts_parse(struct opts *opts, int argc, char *argv[])
{
	static const struct option long_options[] = {
		{"height",      required_argument, NULL, '1'},
		{"output-file", required_argument, NULL, 'o'},
		{"help",        no_argument,       NULL, 'h'},
		{"verbose",     no_argument,       NULL, 'v'},
		{"version",     no_argument,       NULL, 'V'},
		{ NULL,         0,                 NULL, 0},
	};
	static const char short_options[] = "bo:f:hvV";

	*opts = (struct opts){
		.height = 10000.0,
		.output_file = "-",
		.help = opt_no,
		.verbose = opt_no,
		.version = opt_no,
	};

	if (0) {
		int i;

		debug("argc = %d\n", argc);
		for (i = 0; i < argc; i++) {
			debug("  %d: %p = '%s'\n", i, &argv[i],
				argv[i]);
		}
	}
	
	while (1) {
		int c = getopt_long(argc, argv, short_options, long_options,
			NULL);

		if (c == EOF)
			break;

		if (0) {
			debug("%c => '%s'\n", c, optarg);
		}

		switch (c) {
		// flag
		case '1':
			opts->height = to_float(optarg);
			if (opts->height == HUGE_VALF) {
				opts->help = opt_yes;
				return -1;
			}
			break;
		// admin
		case 'o': {
			size_t len;
			
			if (0) {
				debug("p:   %p = '%s'\n", optarg, optarg);
			}
			if (!optarg) {
				error("Missing required argument <output-file>.'\n");
				opts->help = opt_yes;
				return -1;
			}
			len = strlen(optarg) + 1;
			opts->output_file = mem_alloc(len);
			strcpy(opts->output_file, optarg);
			break;
		}
		case 'h':
			opts->help = opt_yes;
			break;
		case 'v':
			opts->verbose = opt_yes;
			break;
		case 'V':
			opts->version = opt_yes;
			break;
		default:
			assert(0);
			opts->help = opt_yes;
			return -1;
		}
	}

	return optind != argc;
}

struct flag_dimensions {
	float height;
	float width;
	float blue_height;
	float blue_width;
	float star_v_grid;
	float star_h_grid;
	float stripe_height;
	float star_diameter;
};

struct flag_colors {
	char red[hex_color_len];
	char white[hex_color_len];
	char blue[hex_color_len];
};

__attribute__((unused)) static const struct flag_colors flag_colors_full = {
	.red = "#b22234",
	.white = "#ffffff",
	.blue = "#3c3b6e",
};

__attribute__((unused)) static const struct flag_colors flag_colors_od = {
	.red = "#706b5c",
	.white = "#d9d9d9",
	.blue = "#585850",
};

static void flag_dimensions_fill(struct flag_dimensions *fd, float height)
{
	fd->height = height;
	fd->width = fd->height * 1.9;
	fd->blue_height = fd->height * 7.0 / 13.0;
	fd->blue_width = fd->width * 2.0 / 5.0;
	fd->star_v_grid = fd->blue_height / 10.0;
	fd->star_h_grid = fd->blue_width / 12.0;
	fd->stripe_height = fd->height / 13.0;
	fd->star_diameter = fd->stripe_height * 4.0 / 5.0;
}

static void write_stars(FILE* out_stream, struct flag_dimensions *fd)
{
	struct star_params star_params;
	struct svg_style style;
	struct svg_transform tform;

	star_params.points = 5;
	star_params.density = 2;
	star_params.radius = fd->star_diameter / 2.0;
	star_params.rotation = -90.0;

	svg_style_set(&style, flag_colors_full.white, NULL, 0);
	//svg_style_set(&style, _hex_color_yellow, NULL, 0);

	tform = null_svg_transform;
	tform.translate.x = fd->star_h_grid;
	tform.translate.y = fd->star_v_grid;

	svg_open_group(out_stream, &style, NULL, "star_group");
	svg_open_group(out_stream, NULL, NULL, "stars_18");
	svg_open_group(out_stream, NULL, NULL, "stars_9");
	svg_open_group(out_stream, NULL, NULL, "stars_5");
	svg_open_group(out_stream, NULL, NULL, "stars_4");
	svg_write_star(out_stream, NULL, &tform, "stars_1", &star_params);

	fprintf(out_stream, "<use xlink:href=\"#stars_1\" y=\"%f\"/>\n",
		2.0 * fd->star_v_grid);
	fprintf(out_stream, "<use xlink:href=\"#stars_1\" y=\"%f\"/>\n",
		4.0 * fd->star_v_grid);
	fprintf(out_stream, "<use xlink:href=\"#stars_1\" y=\"%f\"/>\n",
		6.0 * fd->star_v_grid);
	svg_close_group(out_stream); // stars_4

	fprintf(out_stream, "<use xlink:href=\"#stars_1\" y=\"%f\"/>\n",
		8.0 * fd->star_v_grid);
	svg_close_group(out_stream); // stars_5

	fprintf(out_stream, "<use xlink:href=\"#stars_4\" x=\"%f\" y=\"%f\"/>\n",
		1.0 * fd->star_h_grid, 1.0 * fd->star_v_grid);
	svg_close_group(out_stream); // stars_9

	fprintf(out_stream, "<use xlink:href=\"#stars_9\" x=\"%f\"/>\n",
		2.0 * fd->star_h_grid);
	svg_close_group(out_stream); // stars_18

	fprintf(out_stream, "<use xlink:href=\"#stars_18\" x=\"%f\"/>\n",
		4.0 * fd->star_h_grid);
	fprintf(out_stream, "<use xlink:href=\"#stars_9\" x=\"%f\"/>\n",
		8.0 * fd->star_h_grid);
	fprintf(out_stream, "<use xlink:href=\"#stars_5\" x=\"%f\"/>\n",
		10.0 * fd->star_h_grid);

	svg_close_group(out_stream); // star_group
}

static void write_flag(FILE* out_stream, float height)
{
	struct flag_dimensions fd;
	static const char flag_id[] = "flag_usa_1";
	struct svg_style style;
	struct svg_line sl;
	struct svg_rect sr;
	unsigned int i;

	flag_dimensions_fill(&fd, height);

	debug("%s\n", flag_id);
	debug("height = %f\n", fd.height);
	debug("width = %f\n", fd.width);
	debug("blue_height = %f\n", fd.blue_height);
	debug("blue_width = %f\n", fd.blue_width);
	debug("star_v_grid = %f\n", fd.star_v_grid);
	debug("star_h_grid = %f\n", fd.star_h_grid);
	debug("stripe_height = %f\n", fd.stripe_height);
	debug("star_diameter = %f\n", fd.star_diameter);

	// white_background
	svg_style_set(&style, flag_colors_full.white, NULL, 0);
	//svg_style_set(&style, _hex_color_light_gray, NULL, 0);

	sr.width = fd.width;
	sr.height = fd.height;
	sr.x = 0.0;
	sr.y = 0.0;
	sr.rx = 0.0;
	sr.ry = 0.0;
	svg_write_rect(out_stream, &style, NULL, "white_background", &sr);

	// red_stripes
	svg_style_set(&style, NULL, flag_colors_full.red, fd.stripe_height);

	svg_open_path(out_stream, &style, NULL, "red_stripes");
	fprintf(out_stream, "d=\"\n");
	for (i = 0; i < 7; i++) {
		fprintf(out_stream, "M0,%f H%f\n",
			(0.5 + 2.0 * i) * fd.stripe_height, fd.width);
	}
	fprintf(out_stream, "\"\n");
	svg_close_object(out_stream);

	// blue_background
	svg_style_set(&style, flag_colors_full.blue, NULL, 0);

	sr.width = fd.blue_width;
	sr.height = fd.blue_height;
	svg_write_rect(out_stream, &style, NULL, "blue_background", &sr);

	if (svg_debug_stream()) {
		// v_grid
		style = svg_style_light_green_light_green;
		style.stroke.width = 1.0 + fd.height / 1000.0;

		sl.a.x = 0.0;
		sl.b.x = fd.blue_width;
		sl.a.y = sl.b.y = 0.0;
		for (i = 0; i < 11; i++) {
			svg_write_line(svg_debug_stream(), &style, NULL, "v_grid", &sl);
			sl.a.y = sl.b.y += fd.star_v_grid;
		}

		// h_grid
		style = svg_style_light_green_light_green;
		style.stroke.width = 1.0 + fd.height / 1000.0;

		sl.a.y = 0.0;
		sl.b.y = fd.blue_height;
		sl.a.x = sl.b.x = 0.0;
		for (i = 0; i < 13; i++) {
			svg_write_line(svg_debug_stream(), &style, NULL, "h_grid", &sl);
			sl.a.x = sl.b.x += fd.star_h_grid;
		}
	}
	
	write_stars(out_stream, &fd);

}

static void write_svg(FILE* out_stream, float height)
{
	svg_open_svg(out_stream, NULL);
	//svg_debug_stream_set(out_stream);
	write_flag(out_stream, height);
	svg_close_svg(out_stream);
}

int main(int argc, char *argv[])
{
	struct opts opts;
	FILE *out_stream;

	log_set_exit_on_error(true);

	if (opts_parse(&opts, argc, argv)) {
		print_usage(&opts);
		return EXIT_FAILURE;
	}

	if (opts.version == opt_yes) {
		print_version();
		return EXIT_SUCCESS;
	}

	if (opts.verbose == opt_yes) {
		log_set_verbose(true);
	}

	if (!strcmp(opts.output_file, "-")) {
		out_stream = stdout;
	} else {
		out_stream = fopen(opts.output_file, "w");
		if (!out_stream) {
			error("open <output-file> '%s' failed: %s\n",
				opts.output_file, strerror(errno));
			assert(0);
			return EXIT_FAILURE;
		}
	}

	if (opts.help == opt_yes) {
		print_usage(&opts);
		return EXIT_SUCCESS;
	}

	write_svg(out_stream, opts.height);

	return EXIT_SUCCESS;
}

