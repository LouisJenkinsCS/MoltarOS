#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
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
#include <include/kernel/mem.h>
#include <include/sched/task.h>
#include <include/helpers.h>

uint32_t PHYSICAL_MEMORY_END;
uint32_t PHYSICAL_MEMORY_START;

uint32_t STACK_START;

static void thread_task(void *UNUSED(args)) {

	for (;;) {
		asm volatile ("cli");
	
		uint32_t x = vga_get_x();
		uint32_t y = vga_get_y();
		vga_set_x(70);
		vga_set_y(0);
		rtc_print();
		vga_set_x(x);
		vga_set_y(y);

		asm volatile ("sti");

		yield();
	}
}

void kernel_init(struct multiboot_info *info, uint32_t esp) {
	STACK_START = esp;
	vga_init();
	KTRACE("Stack Start: %x", esp);
	KINFO("Virtual Memory (Paging) Initialized...");
	KINFO("Video Graphics Array (VGA) Initialized...");
	gdt_init();
	KINFO("Global Descriptor Table (GDT) Initialized...");
	idt_init();
	KINFO("Interrupt Descriptor Table (IDT) Initialized...");
	timer_init();
	KINFO("System Timer (PIT) Initialized...");
	if (!multiboot_RAM(info, &PHYSICAL_MEMORY_START, &PHYSICAL_MEMORY_END)) {
		KPANIC("Failed to detect physical memory (RAM)!!!");
	}
	KDEBUG("Detected => RAM {Start: %d, End: %d, Total: %d}", PHYSICAL_MEMORY_START, PHYSICAL_MEMORY_END, PHYSICAL_MEMORY_END - PHYSICAL_MEMORY_START);
	mem_init();
	KINFO("Memory Heap and Allocators (kmalloc & kfree) Initialized...");
	vga_dynamic_init();
}

void kernel_main(void) {
	KINFO("Initializing Multitasking...");
	task_init();
	thread_create(thread_task, NULL);

	keyboard_init();
	KINFO("Keyboard Initialized...");
	KINFO("Kernel Fully Initialized!");

	// Loop infinitely
	while (true) {
		asm volatile ("hlt");
	}
}
