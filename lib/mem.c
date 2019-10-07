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
#include <string.h>

#include "log.h"
#include "mem.h"

static const unsigned int header_pattern = 0xaabbccddU;
static const unsigned int footer_pattern = 0xccbbaa99U;

struct mem_footer {
	unsigned int guard;
};

struct mem_header {
	unsigned int guard1;
	size_t size;
	struct mem_footer *footer;
	bool freed;
	unsigned int guard2;
};

static struct mem_header *to_header(void *p)
{
	return (struct mem_header *)p - 1;
}

void *mem_alloc(size_t size)
{
	struct mem_header *header;
	struct mem_footer *footer;
	void *p;

	header = malloc(sizeof(struct mem_header) + size +
		sizeof(struct mem_footer));

	if (!header) {
		error("Malloc %lu failed: %s.\n", (unsigned long)size,
			strerror(errno));
		assert(0);
		exit(EXIT_FAILURE);
	}

	p = header + 1;

	//debug("sizeof = %lu, header = %p, p = %p\n",
	//	sizeof(struct mem_header), header, p);
	
	memset(p, 0, size);

	footer = p + size;
	footer->guard = footer_pattern;

	header->guard1 = header_pattern;
	header->size = size;
	header->footer = footer;
	header->freed = false;
	header->guard2 = header_pattern;

	return p;
}

void *mem_realloc(void *p, size_t size)
{
	void *n = realloc(p, size);

	error("TODO: Need to add header.\n");
	assert(0);
	exit(EXIT_FAILURE);

	if (!n) {
		error("Realloc %lu failed: %s.\n", (unsigned long)size,
			strerror(errno));
		assert(0);
		exit(EXIT_FAILURE);
	}
	return n;
}

void mem_free(void *p)
{
	struct mem_header *header;

	if (!p) {
		error("Null free.\n");
		assert(0);
		exit(EXIT_FAILURE);
	}

	header = to_header(p);

	if (header->guard1 != header_pattern ||
		header->guard2 != header_pattern) {
		error("Bad header guard.\n");
		assert(0);
		exit(EXIT_FAILURE);
	}

	if (header->freed == true) {
		error("Double free.\n");
		assert(0);
		exit(EXIT_FAILURE);
	}

	if (header->footer->guard != footer_pattern) {
		error("Bad footer guard, buffer over run.\n");
		assert(0);
		exit(EXIT_FAILURE);
	}

	header->freed = true;

	free(header);
}
