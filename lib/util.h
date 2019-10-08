/*
 *  moto-design SGV utils.
 */

#if ! defined(_MD_GENERATOR_UTIL_H)
#define _MD_GENERATOR_UTIL_H

//#include <stdbool.h>

const char *eat_front_ws(const char *p);
void eat_tail_ws(char *p);

unsigned int to_unsigned(const char *str);
float to_float(const char *str);

int random_int(int min, int max);
unsigned int random_unsigned(unsigned int min, unsigned int max);
float random_float(float min, float max);
unsigned int *random_array(unsigned int len);

static inline float min_f(float a, float b)
{
	return a < b ? a : b;
}

static inline float max_f(float a, float b)
{
	return a > b ? a : b;
}

#endif /* _MD_GENERATOR_UTIL_H */
