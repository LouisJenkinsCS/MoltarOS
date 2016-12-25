#include <stdio.h>

#ifdef __IS_MOLTAROS
#include "../../kernel/include/drivers/vga.h"
#endif

int putchar(int c) {
#ifdef __IS_MOLTAROS
	vga_putc((char) c);
#else
	// TODO: Write syscall
#endif

	return c;
}
