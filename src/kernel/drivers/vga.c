#include <stdint.h>
#include <string.h>

#include <include/kernel/logger.h>
#include <include/ds/array.h>
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


static array_t *lines;
static size_t line_number = 0;

static inline uint8_t make_color(enum vga_color foreground, enum vga_color background);

static inline uint16_t color_char(const char c);

static inline size_t get_index();

static inline void clear_line(size_t line);

static void save_buffer();

static void draw_buffer();

static void update_cursor();

static bool reserved(size_t i, size_t j);

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
	// Create our buffer of lines.
	lines = array_create(vga_height);

	// Create the initial 25 lines we require to buffer them
	for (size_t i = 0; i < vga_height; i++) {
		array_add(lines, kmalloc(sizeof(uint16_t) * vga_width));
	}
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
		while(x < vga_width && !reserved(x, y)) {
			size_t idx = get_index();
			buf[idx] = color_char(' ');
			x++;
		}
	} else {
		// On reserved spots, handle as a newline character, and then print
		if (reserved(x, y)) {
			vga_putc('\n');
			vga_putc(c);
			return;
		}

		char ch = (c == '\t') ? ' ' : c;
		size_t idx = get_index();
		buf[idx] = color_char(ch);
	}

	/*
		Below to prevent overflowing the VGA buffer, we instead restart at the beginning the next
		line, and if it is the last line, at the beginning of the first row and column. For now
		this is adequate, but in the future, there will be support for scrolling up and down using
		the keyboard (once the driver is implemented) and storing the previous data as well
		(once memory management and the heap is implemented).
	*/
	if(++x >= vga_width || reserved(x, y)) {
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
	// Preserve current contents.
	save_buffer();

	// If there is no next line, then we did not save one, and hence we need to allocate
	// a new one. However, if there IS a next line, we can easily just display that one.
	if (lines->used <= line_number + vga_height) {
		uint16_t *line = kmalloc(sizeof(uint16_t) * vga_width);

		// Fill with blank lines, so it can easily be displayed
		for (size_t i = 0; i < vga_width; i++) {
			line[i] = color_char(' ');
		}

		// Add our new line to the list of lines; Now we can seamlessly obtain the line
		// to display without extra logic.
		array_add(lines, line);
	}

	// Scroll down
	line_number++;
	draw_buffer();
}

void vga_scroll_up() {
	// No more lines to scroll
	if (line_number == 0) {
		return;
	}

	// Preserve current contents.
	save_buffer();

	// Scroll up
	line_number--;
	draw_buffer();
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
	if (y > vga_height || x > vga_width) {
		size_t oldX = x;
		size_t oldY = y;
		y = 0;
		x = 0;
		KPANIC("Bad VGA Coordinates: x=%d, y=%d", oldX, oldY);
	}
	return (y * vga_width) + x;
}

static void clear_line(size_t line) {
	for(size_t i = 0; i < vga_width; i++)
		buf[(line * vga_width) + i] = color_char(' ');
}

static bool reserved(size_t i, size_t j) {
	// TODO: Add 2nd line for key press
	return (i == 0 && j == vga_width - 10) || (i == 1 && j == vga_width - 15);
}

static void draw_buffer() {
	// We simply draw vga_height lines with respect to our current line_number
	for (size_t i = 0; i < vga_height; i++) {
		// Get current line to draw
		uint16_t *line = array_get(lines, line_number + i);

		// Copy the current line into vga buffer
		for (size_t j = 0; j < vga_width; j++) {
			if (reserved(i, j)) {
				break;
			}

			buf[(i * vga_width) + j] = line[j];
		}
	}
}

static void save_buffer() {
	// Copy the current buffer's contents (making sure to not copy
	// reserved memory) into our own buffer
	for(size_t i = 0; i < vga_height; i++) {
		// Get current line buffer
		uint16_t *line = array_get(lines, line_number + i);

		for(size_t j = 0; j < vga_width; j++) {
			// Reserved...
			// TODO: Add 2nd line for key press
			if (reserved(i, j)) {
				break;
			}

			// Copy contents
			line[j] = buf[(i * vga_width) + j];
		}
	}
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