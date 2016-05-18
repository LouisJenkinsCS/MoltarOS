#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define __IS_MOLTAROS 1

#include <kernel/vga.h>

const char *msg = "When are we getting ready to leave?";

void kernel_init() {
	vga_init();
}

#define STRINGIFY_THIS(x) #x

#define STRINGIFY(x) STRINGIFY_THIS(x)

void kernel_main() {
	printf("[%s](%s:%s): \"%s\"", __FILE__, __FUNCTION__, STRINGIFY(__LINE__), msg);
}