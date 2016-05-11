#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
	The VGA expects these values for a particular color to be shown graphically.
	The foreground color denotes the color of the actual text, and is the first 
	4 bits, while the background color must be the last 4 bytes.

	Hence a background GREEN(2) and foreground of BLUE(1) would need to be declared
	as such...

	1 | 2 << 4

	or

	BLUE | GREEN << 4

	Which is done for us in the helper inline function.
*/
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

static struct {
	/// The VGA text-mode buffer
	uint16_t *BUFFER;
	/// The current dimension of the buffer (X and Y)
	struct {
		size_t X;
		size_t Y;
	} COORDS;
	/// Background and Foreground color of the VGA output
	uint16_t COLOR;
} VGA_TEXT_BUFFER;

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

/*
	Combines foreground and background color into one
*/
static inline uint8_t vga_color_make(enum vga_color foreground, enum vga_color background) {
	return foreground | background << 4;
}

static inline uint16_t vga_entry_make(char c, uint8_t color) {
	uint16_t c16 = c;
	uint16_t color16 = color;

	return c16 | color16 << 8;
}

size_t strlen(const char *str) {
	size_t ret = 0;

	while(*str++)
		ret++;

	return ret;
}

static void vga_init() {
	VGA_TEXT_BUFFER.BUFFER = (uint16_t *) 0xB8000;
	VGA_TEXT_BUFFER.COORDS.X = VGA_TEXT_BUFFER.COORDS.Y = 0;
	VGA_TEXT_BUFFER.COLOR = vga_color_make(COLOR_LIGHT_GREY, COLOR_BLACK);

	// Clear VGA buffer of all entries
	for(size_t y = 0; y < VGA_HEIGHT; y++) {
		for(size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			VGA_TEXT_BUFFER.BUFFER[index] = vga_entry_make(' ', VGA_TEXT_BUFFER.COLOR);
		}
	}
}

static void vga_color_set(uint8_t color) {
	VGA_TEXT_BUFFER.COLOR = color;

	// Change the color but keep the contents
	for(size_t y = 0; y < VGA_HEIGHT; y++) {
		for(size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			VGA_TEXT_BUFFER.BUFFER[index] = vga_entry_make(VGA_TEXT_BUFFER.BUFFER[index], VGA_TEXT_BUFFER.COLOR);
		}
	}
}

static void vga_entry_put_at(char c, uint8_t color, size_t x, size_t y) {
	if(x >= VGA_WIDTH || y >= VGA_HEIGHT || c == '\n')
		return;

	const size_t index = y * VGA_WIDTH + x;
	VGA_TEXT_BUFFER.BUFFER[index] = vga_entry_make(c, color);
}

static void vga_entry_put(char c) {
	/*
		Print the character to the VGA Text buffer; if it is a newline, it will fill the rest of the line
		with spaces to simulate an actual newline.
	*/
	if(c == '\n')
		while(VGA_TEXT_BUFFER.COORDS.X != (VGA_WIDTH - 1))
			vga_entry_put_at(' ', VGA_TEXT_BUFFER.COLOR, VGA_TEXT_BUFFER.COORDS.X++, VGA_TEXT_BUFFER.COORDS.Y);
	else
		vga_entry_put_at(c, VGA_TEXT_BUFFER.COLOR, VGA_TEXT_BUFFER.COORDS.X, VGA_TEXT_BUFFER.COORDS.Y);

	/*
		Below to prevent overflowing the VGA buffer, we instead restart at the beginning the next
		line, and if it is the last line, at the beginning of the first row and column. For now
		this is adequate, but in the future, there will be support for scrolling up and down using
		the keyboard (once the driver is implemented) and storing the previous data as well
		(once memory management and the heap is implemented).
	*/
	if(++VGA_TEXT_BUFFER.COORDS.X == VGA_WIDTH) {
		VGA_TEXT_BUFFER.COORDS.X = 0;

		if(++VGA_TEXT_BUFFER.COORDS.Y == VGA_HEIGHT) {
			VGA_TEXT_BUFFER.COORDS.Y = 0;
		}
	}
}

static void vga_entry_write(const char *data) {
	while(*data)
		vga_entry_put(*data++);
}

void kmain() {
	// Intialize the VGA_TEXT_BUFFER
	vga_init();

	for(size_t i = 0; i < VGA_HEIGHT; i++)
		vga_entry_write("Original Line...\n");

	vga_entry_write("Overwritten Line!\n");

	vga_entry_write("Second Overwritten Line!\n");

	vga_entry_write("\nSkipped Line!\n");
}