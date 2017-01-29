#ifndef MOLTAROS_VGA_H
#define MOLTAROS_VGA_H

#include <stddef.h>

enum vga_color {
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
	COLOR_WHITE = 15
};

extern const size_t vga_width;
extern const size_t vga_height;

#define VGA_RESERVED_KBD 1
#define VGA_RESERVED_TICK 2

void vga_init();

void vga_dynamic_init();

/*
	Sets the VGA buffer's background and foreground color.
*/
void vga_set_color(enum vga_color foreground, enum vga_color background);

void vga_print(const char *str);

void vga_print_reserved(const char *str, int type);

void vga_putc(const char c);

void vga_clear();

void vga_clear_line();

void vga_scroll_down();

void vga_scroll_up();

void vga_set_x(size_t x);

void vga_set_y(size_t y);

size_t vga_get_x();

size_t vga_get_y();

#endif /* MOLTAROS_VGA_H */
