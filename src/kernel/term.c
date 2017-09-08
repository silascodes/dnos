
#include "kstring.h"
#include "term.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static size_t kterm_row;
static size_t kterm_column;
static uint8_t kterm_color;
static uint16_t* kterm_buffer;

static uint8_t make_color(enum kterm_color fg, enum kterm_color bg) {
	return fg | bg << 4;
}
 
static uint16_t make_vgaentry(char c, uint8_t color) {
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}
 
void kterm_init() {
	kterm_row = 0;
	kterm_column = 0;
	kterm_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	kterm_buffer = (uint16_t*) 0xB8000;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			kterm_buffer[index] = make_vgaentry(' ', kterm_color);
		}
	}
}
 
void kterm_setcolor(uint8_t pColor) {
	kterm_color = pColor;
}
 
void kterm_putat(char pChar, uint8_t pColor, size_t pX, size_t pY) {
	const size_t index = pY * VGA_WIDTH + pX;
	kterm_buffer[index] = make_vgaentry(pChar, pColor);
}
 
void kterm_putchar(char pChar) {
	if(pChar == '\n' || pChar == '\r') {
		kterm_column = 0;
		kterm_row++;
	}
	else {
		const size_t index = kterm_row * VGA_WIDTH + kterm_column;
		kterm_buffer[index] = make_vgaentry(pChar, kterm_color);
		
		if(++kterm_column == VGA_WIDTH) {
			kterm_column = 0;
			kterm_row++;
		}
	}
	
	if(kterm_row == VGA_HEIGHT) {
		kterm_row = 0;
	}
}

void kterm_puts(const char *pStr) {
	size_t len = kstrlen(pStr);
	for(size_t i = 0; i < len; i++) {
		kterm_putchar(pStr[i]);
	}
}

