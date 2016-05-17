#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define __IS_MOLTAROS 1

#include <kernel/vga.h>

void kernel_init() {
	// TODO
}

void kernel_main() {
	vga_init();

	for(size_t i = 0; i < vga_height - 1; i++)
		vga_print("Original Line...\n");

	vga_print("Overwritten Line!\n");
	vga_print("Second Overwritten Line!\n");
	vga_print("\nSkipped Line!\n");

	printf("%s %s %s: Okay working good: %c+\n", "Testing", "Printf", "Functionality", 'A');
	printf("Quickly made a change!\n");
	printf("Made another quick change!");
}