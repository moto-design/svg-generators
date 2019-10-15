/*
 *  moto-design SGV utils.
 */

#define _GNU_SOURCE
#define _ISOC99_SOURCE

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

static bool exit_on_error = false;
static bool verbose_state = false;

void log_set_exit_on_error(bool state)
{
	exit_on_error = state;
}

void log_set_verbose(bool state)
{
	verbose_state = state;
}

bool log_get_verbose(void)
{
	return verbose_state;
}

void  __attribute__((unused)) _error(const char *func, int line,
	const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "ERROR: %s:%d: ", func, line);

	va_start(ap, fmt);

	vfprintf(stderr, fmt, ap);
	fflush(stderr);

	va_end(ap);

	if (exit_on_error) {
		exit(EXIT_FAILURE);
	}

}

void  __attribute__((unused)) _log(const char *func, int line,
	const char *fmt, ...)
{
	va_list ap;
	
	if (!verbose_state) {
		return;
	}

	fprintf(stderr, "%s:%d: ", func, line);

	va_start(ap, fmt);

	vfprintf(stderr, fmt, ap);
	fflush(stderr);

	va_end(ap);
}

void  __attribute__((unused)) _warn(const char *func, int line,
	const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "WARNING: %s:%d: ", func, line);

	va_start(ap, fmt);

	vfprintf(stderr, fmt, ap);
	fflush(stderr);

	va_end(ap);
}
