#include <stdint.h>
#include <string.h>

#include <include/ds/list.h>
#include <include/drivers/vga.h>
#include <include/x86/io_port.h>
#include <include/helpers.h>
#include <include/kernel/mem.h>

/* X and Y coordinates for current position in VGA buffer */
static size_t x, y;

/* The current VGA color mask */
static uint8_t color;

/* Pointer to VGA text-mode buffer */
static uint16_t *buf;

/* VGA maximum width and height */
const size_t vga_height = 25;
const size_t vga_width = 80;


static list_t *above_lines;
static list_t *below_lines;

static inline uint8_t make_color(enum vga_color foreground, enum vga_color background);

static inline uint16_t color_char(const char c);

static inline size_t get_index();

static inline void clear_line(size_t line);

static void update_cursor();

/*
	Initializes the buffer's attributes above to their proper values, should be called before kmain().
*/
void vga_init() {
	buf = (uint16_t *) 0xC00B8000;
	x = y = 0;
	color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);

	// Clear VGA buffer of all entries
	for(size_t i = 0; i < vga_height; i++)
		for(size_t j = 0; j < vga_width; j++)
			buf[(i * vga_width) + j] = color_char(' ');
}

// Initializes rest of components to support scrolling.
void vga_dynamic_init() {
	below_lines = list_create();
	above_lines = list_create();
}

void vga_print_color(enum vga_color new_color,const char *str) {
	uint8_t old_color = color;
	color = make_color(new_color, COLOR_BLACK);
	vga_print(str);
	color = old_color;
}

void vga_set_color(enum vga_color foreground, enum vga_color background) {
	color = make_color(foreground, background);

	// Change color of all characters currently in buffer.
	for(size_t i = 0; i < vga_height; i++) {
		for(size_t j = 0; j < vga_width; j++) {
			const size_t index = (i * vga_width) + j;
			buf[index] = color_char((char) buf[index]);
		}
	}
}

void vga_print(const char *str) {
	while(*str)
		vga_putc(*str++);
}

void vga_print_reserved(const char *str, int type) {
	vga_set_x(60);
	vga_set_y(type);
	
	size_t len = MIN(strlen(str), 20);
	memcpy(buf + type * vga_width + 60, str, len);
	memset(buf + type * vga_width + 60 + len, 0, 20 - len);
}

void vga_putc(const char c) {
	if(c == '\n') {
		// Fill rest of line with empty space
		while(x < vga_width) {
			size_t idx = get_index();
			buf[idx] = color_char(' ');
			x++;
		}
	} else {
		size_t idx = get_index();
		buf[idx] = color_char(c);
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

	update_cursor();
}

void vga_clear() {
	for(size_t i = 0; i < vga_height; i++)
		clear_line(y);
}

void vga_clear_line() {
	clear_line(y);
}

void vga_scroll_down() {
	// Preserve the old line's value.
	uint16_t *line = kmalloc(2 * vga_width);
	for (size_t i = 0; i < vga_width - 12; i++) {
		line[i] = buf[i];
	}
	list_back(above_lines, line);

	for(size_t i = 1; i < vga_height; i++) {
		for(size_t j = 0; j < vga_width; j++) {
			// Previous and current line indexes
			const size_t prev = ((i-1) * vga_width) + j;
			const size_t curr = (i * vga_width) + j;

			// The contents of the current line is moved to the previous line
			buf[prev] = buf[curr];
		}
	}

	// If there is a buffered line below, restore it...
	if (list_current(below_lines)) {
		line = list_current(below_lines);
		list_remove(below_lines, NULL);

		for (size_t i = 0; i < vga_width; i++) {
			buf[((vga_height - 1) * vga_width) + i] = line[i];
		}
		kfree(line);
	} else {
		clear_line((vga_height - 1));
	}
}

void vga_scroll_up() {
	// No line to scroll
	if (!list_current(above_lines)) {
		return;
	}

	// Preserve the old line's value.
	uint16_t *line = kmalloc(2 * vga_width);
	for (size_t i = 0; i < vga_width; i++) {
		line[i] = buf[((vga_height - 1) * vga_width) + i];
	}
	list_back(below_lines, line);

	for(size_t i = vga_height - 2; i < vga_height; i--) {
		for(size_t j = 0; j < vga_width; j++) {
			if (i == 0 && j == vga_width - 12) {
				break;
			}
			// Previous and current line indexes
			const size_t next = ((i+1) * vga_width) + j;
			const size_t curr = (i * vga_width) + j;

			// The contents of the current line is moved to the previous line
			buf[next] = buf[curr];
		}
	}

	// If there is a buffered line above, restore it
	if (list_current(above_lines)) {
		line = list_current(above_lines);
		list_remove(above_lines, NULL);


		for (size_t i = 0; i < vga_width; i++) {
			buf[i] = line[i];
		}

		kfree(line);
	} else {
		// Nothing to scroll
		return;
	}
}

void vga_set_x(size_t _x) {
	if(x < vga_width)
		x = _x;

	update_cursor();
}

void vga_set_y(size_t _y) {
	if(y < vga_height)
		y = _y;

	update_cursor();
}

size_t vga_get_x() {
	return x;
}

size_t vga_get_y() {
	return y;
}



static uint8_t make_color(enum vga_color foreground, enum vga_color background) {
	return (uint8_t) (foreground | background << 4);
}

static uint16_t color_char(const char c) {
	uint16_t c16 = (uint16_t) c;
	uint16_t color16 = color;

	return (uint16_t) (c16 | color16 << 8);
}

static inline size_t get_index() {
	return (y * vga_width) + x;
}

static void clear_line(size_t line) {
	for(size_t i = 0; i < vga_width; i++)
		buf[(line * vga_width) + i] = color_char(' ');
}

static void update_cursor() {
		uint16_t pos = (uint16_t) get_index();
		uint8_t low = pos & 0xFF;
		uint8_t high = (pos >> 8) & 0xFF;

		// Set the lower 8-bits of the cursor position
		outb(0x3D4, 0x0F);
		outb(0x3D5, low);

		// Set the higher 8-bits of the cursor position
		outb(0x3D4, 0x0E);
		outb(0x3D5, high);
}