
#ifndef _KTERM_H
#define _KTERM_H

#include <stdint.h>
#include <stddef.h>

/* Hardware text mode color constants. */
enum kterm_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

void kterm_init();
void kterm_setcolor(uint8_t pColor);
void kterm_putat(char pChar, uint8_t pColor, size_t pX, size_t pY);
void kterm_putchar(char pChar);
void kterm_puts(const char *pStr);

#endif

