#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define __IS_MOLTAROS 1

#include <include/vga.h>
#include <include/gdt.h>
#include <include/idt.h>
#include <include/timer.h>

const char *msg = "When are we getting ready to leave?";

void kernel_init() {
	vga_init();
	gdt_init();
	idt_init();
}

static void kernel_tick(struct registers *regs) {
	(void) regs;
	static uint8_t ticks = 0;
	static uint32_t seconds = 0;

	if(++ticks == 20) {
		printf("%d seconds went by...\n", ++seconds);
		ticks = 0;
	}
}

#define STRINGIFY_THIS(x) #x

#define STRINGIFY(x) STRINGIFY_THIS(x)

void kernel_main() {
	printf("[%s](%s:%s): \"%s\"\n", __FILE__, __FUNCTION__, STRINGIFY(__LINE__), msg);
	init_timer(20, kernel_tick);

	asm volatile ("sti");

	while(true)
		asm volatile ("hlt");

}