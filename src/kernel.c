#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "vga.h"


void kmain() {
	vga_init();

	for(size_t i = 0; i < vga_height; i++)
		vga_print("Original Line...\n");

	vga_print("Overwritten Line!\n");

	vga_print("Second Overwritten Line!\n");

	vga_print("\nSkipped Line!\n");
}