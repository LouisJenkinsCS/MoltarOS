#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/kernel/vga.h"

void kernel_init() {
	// TODO
}

void kernel_main() {
	vga_init();

	for(size_t i = 0; i < vga_height; i++)
		vga_print("Original Line...\n");

	vga_print("Overwritten Line!\n");

	vga_print("Second Overwritten Line!\n");

	vga_print("\nSkipped Line!\n");
}