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
#include <errno.h>
#include <limits.h>
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "mem.h"
#include "util.h"

const char *eat_front_ws(const char *p)
{
	//char *const start = p;

	assert(p);

	while (*p && (*p == ' ' || *p == '\t' || *p == '\r')) {
		//debug("'%s': remove='%c'\n",
		//	start, *p);
		p++;
	}

	return p;
}

void eat_tail_ws(char *p)
{
	char *const start = p;

	assert(p);

	//debug("> '%s'\n", p);
	while (*p) {
		p++;
	}
	p--;

	while (p > start && (*p == ' ' || *p == '\t' || *p == '\r')) {
		//debug("'%s': remove='%c'\n",
		//	start, *p);
		p--;
	}
	
	*(p + 1) = 0;
	//debug("< '%s'\n", start);
}

unsigned int to_unsigned(const char *str)
{
	const char *p;
	unsigned long u;

	for (p = str; *p; p++) {
		if (!isdigit(*p)) {
			error("isdigit failed: '%s'\n", str);
			return UINT_MAX;
		}
	}

	u = strtoul(str, NULL, 10);

	if (u == ULONG_MAX) {
		error("strtoul '%s' failed: %s\n", str, strerror(errno));
		return UINT_MAX;
	}
	
	if (u > UINT_MAX) {
		error("too big: %lu\n", u);
		return UINT_MAX;
	}

	return (unsigned int)u;
}

float to_float(const char *str)
{
	const char *p;
	const char *start;
	char *end;
	float f;
	bool found_decimal = false;

	assert(str);
	debug("str = @%s@\n", str);

	start = eat_front_ws(str);

	for (p = (*start != '-') ? start : start + 1; *p; p++) {
		if (*p == '-') {
			error("bad sign: '%s'\n", str);
			return HUGE_VALF;
		}
		if (*p == '.') {
			if (found_decimal) {
				error("multiple decimal: '%s'\n", str);
				return HUGE_VALF;
			}
			found_decimal = true;
			continue;
		}		
		if (!isdigit(*p)) {
			error("isdigit failed: '%s'\n", str);
			return HUGE_VALF;
		}
	}

	f = strtof(start, &end);

	if ((f == 0.0 && end == str) || f == HUGE_VALF) {
		error("strtof '%s' failed: %s\n", str, strerror(errno));
		return HUGE_VALF;
	}

	return f;
}

int random_int(int min, int max)
{
	return min + (rand() % (max - min + 1));
}

unsigned int random_unsigned(unsigned int min, unsigned int max)
{
	return (unsigned int)(min + (rand() % (max - min + 1)));
}

float random_float(float min, float max)
{
    return min + (float)rand() / (float)RAND_MAX * (max - min);
}

unsigned int *random_array(unsigned int len)
{
	unsigned int *p;
	unsigned int i;

	p = mem_alloc(len * sizeof(*p));

	for (i = 0; i < len; i++) {
		p[i] = i;
	}

	for (i = 0; i < len; i++) {
		unsigned int j;
		unsigned int tmp;

		j = rand() % len;
		tmp = p[i];
		p[i] = p[j];
		p[j] = tmp;
	}

	return p;
}
