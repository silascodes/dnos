
#ifndef _KSTDLIB_H
#define _KSTDLIB_H

#include <stdint.h>

char *kitoa(int32_t value, char * str, int base);
char *klitoa(int64_t value, char * str, int base);
char *kutoa(uint32_t value, char * str, int base);
char *klutoa(uint64_t value, char * str, int base);

#endif

