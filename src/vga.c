#include <stdint.h>

#include "vga.h"

/* X and Y coordinates for current position in VGA buffer */
static size_t x, y;

/* The current VGA color mask */
static uint8_t color;

/* Pointer to VGA text-mode buffer */
static uint16_t *buf;

/* VGA maximum width and height */
const size_t vga_height = 25;
const size_t vga_width = 80;



static inline uint8_t make_color(enum vga_color foreground, enum vga_color background);

static inline uint16_t color_char(const char c);

static inline size_t get_index();

static inline void clear_line(size_t line);



/*
	Initializes the buffer's attributes above to their proper values, should be called before kmain().
*/
void vga_init() {
	buf = (uint16_t *) 0xB8000;
	x = y = 0;
	color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);

	// Clear VGA buffer of all entries
	for(size_t i = 0; i < vga_height; i++)
		for(size_t j = 0; j < vga_width; j++)
			buf[(i * vga_width) + j] = color_char(' ');
}



void vga_set_color(enum vga_color foreground, enum vga_color background) {
	color = make_color(foreground, background);

	// Change color of all characters currently in buffer.
	for(size_t i = 0; i < vga_height; i++) {
		for(size_t j = 0; j < vga_width; j++) {
			const size_t index = (i * vga_width) + j;
			buf[index] = color_char(buf[index]);
		}
	}
}

void vga_print(const char *str) {
	while(*str)
		vga_putc(*str++);
}

void vga_putc(const char c) {
	if(c == '\n') {
		// Fill rest of line with empty space
		while(x < vga_width) {
			buf[get_index()] = color_char(' ');
			x++;
		}
	} else {
		buf[get_index()] = color_char(c);
	}

	/*
		Below to prevent overflowing the VGA buffer, we instead restart at the beginning the next
		line, and if it is the last line, at the beginning of the first row and column. For now
		this is adequate, but in the future, there will be support for scrolling up and down using
		the keyboard (once the driver is implemented) and storing the previous data as well
		(once memory management and the heap is implemented).
	*/
	if(++x >= vga_width) {
		x = 0;

		if(++y >= vga_height) {
			y--;
			vga_scroll_down();
		}
	}
}

void vga_clear() {
	for(size_t i = 0; i < vga_height; i++)
		clear_line(y);
}

void vga_scroll_down() {
	for(size_t i = 1; i < vga_height; i++) {
		for(size_t j = 0; j < vga_width; j++) {
			// Previous and current line indexes
			const size_t prev = ((i-1) * vga_width) + j;
			const size_t curr = (i * vga_width) + j;

			// The contents of the current line is moved to the previous line
			buf[prev] = buf[curr];
		}
	}

	clear_line(y);
}



static uint8_t make_color(enum vga_color foreground, enum vga_color background) {
	return foreground | background << 4;
}

static uint16_t color_char(const char c) {
	uint16_t c16 = c;
	uint16_t color16 = color;

	return c16 | color16 << 8;
}

static size_t get_index() {
	return (y * vga_width) + x;
}

static void clear_line(size_t line) {
	for(size_t i = 0; i < vga_width; i++)
		buf[(line * vga_width) + i] = color_char(' ');
}