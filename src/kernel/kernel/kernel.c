#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define __IS_MOLTAROS 1

#include <include/kernel/vga.h>
#include <include/kernel/gdt.h>
#include <include/kernel/idt.h>

const char *msg = "When are we getting ready to leave?";

void kernel_init() {
	vga_init();
	gdt_init();
	idt_init();
}

#define STRINGIFY_THIS(x) #x

#define STRINGIFY(x) STRINGIFY_THIS(x)

void kernel_main() {
	printf("[%s](%s:%s): \"%s\"\n", __FILE__, __FUNCTION__, STRINGIFY(__LINE__), msg);
}