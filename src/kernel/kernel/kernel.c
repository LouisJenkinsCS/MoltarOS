#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define __IS_MOLTAROS 1

#include <include/drivers/vga.h>
#include <include/x86/gdt.h>
#include <include/x86/idt.h>
#include <include/drivers/timer.h>
#include <include/drivers/kbd.h>
#include <include/drivers/rtc.h>
#include <include/kernel/multiboot.h>
#include <include/kernel/logger.h>
#include <include/mm/page.h>
#include <include/helpers.h>

uint32_t PHYSICAL_MEMORY_END;
uint32_t PHYSICAL_MEMORY_START;

size_t ticks_x, ticks_y, time_x, time_y;

void kernel_init(struct multiboot_info *info) {
	vga_init();
	gdt_init();
	idt_init();
	timer_init();
	bool retval = multiboot_RAM(info, &PHYSICAL_MEMORY_START, &PHYSICAL_MEMORY_END);
	KLOG("RAM Stats: Available: %d, Start: %d, End: %d", retval, PHYSICAL_MEMORY_START, PHYSICAL_MEMORY_END);
}

static void kernel_tick(struct registers *UNUSED(regs)) {
	KFRAME;
	static uint32_t ticks = 0;

	vga_set_x(ticks_x);
	vga_set_y(ticks_y);
	printf("%d", ++ticks);

	if(ticks % 1000 == 0) {
		vga_set_x(time_x);
		vga_set_y(time_y);
		rtc_print();
		vga_putc(' ');
	}
}

static void kernel_clock_test() {
	KFRAME;
	printf("Operating System: MoltarOS\nKernel: Moltar\nVersion: 0.001a\nTime: ");
	time_x = vga_get_x();
	time_y = vga_get_y();
	rtc_print();

	printf("\nTesting Clock at 1KHz...\nTicks: 0");
	ticks_x = vga_get_x() - 1;
	ticks_y = vga_get_y();

	timer_set_handler(1000, kernel_tick);

	asm volatile ("sti");

	while(true)
		asm volatile ("hlt");
}

static void kernel_keyboard_test() {
	asm volatile ("sti");

	while (true) {
		asm volatile ("hlt");
	}
}

void kernel_main(void) {
	KFRAME;
	// KLOG("Initializing Keyboard...");
	// keyboard_init();
	page_init();
	kernel_clock_test();
	// KLOG("Initiating Keyboard Test...");
	// kernel_keyboard_test();

}
