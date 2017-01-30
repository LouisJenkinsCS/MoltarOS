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
#include <include/sched/process.h>
#include <include/helpers.h>

uint32_t PHYSICAL_MEMORY_END;
uint32_t PHYSICAL_MEMORY_START;

size_t ticks_x, ticks_y, time_x, time_y;
static bool timer_done = false;

void kernel_init(struct multiboot_info *info) {
	vga_init();
	KLOG("Virtual Memory (Paging) Initialized...");
	KLOG("Video Graphics Array (VGA) Initialized...");
	gdt_init();
	KLOG("Global Descriptor Table (GDT) Initialized...");
	idt_init();
	KLOG("Interrupt Descriptor Table (IDT) Initialized...");
	timer_init();
	KLOG("System Timer (PIT) Initialized...");
	if (!multiboot_RAM(info, &PHYSICAL_MEMORY_START, &PHYSICAL_MEMORY_END)) {
		KPANIC("Failed to detect physical memory (RAM)!!!");
	}
	KLOG("Detected => RAM {Start: %d, End: %d, Total: %d}", PHYSICAL_MEMORY_START, PHYSICAL_MEMORY_END, PHYSICAL_MEMORY_END - PHYSICAL_MEMORY_START);
	mem_init();
	KLOG("Memory Heap and Allocators (kmalloc & kfree) Initialized...");
	vga_dynamic_init();
}

// Memory test that purposefully performs allocation-heavy allocations to test the state of the heap and virtual memory
// implementation. It works by filling up half of the amount of RAM with a deterministic sequence (for debugging) of
// values. On each iteration, we resize by allocating and copying into a new container. 
static void memtest() {
	uint32_t total_memory = PHYSICAL_MEMORY_END - PHYSICAL_MEMORY_START;
	uint32_t total_alloc = 0;
	uint32_t alloc_size = 1;
	uint32_t **allocated_data = NULL;
	uint32_t num_alloc = 1;

	printf("Allocated: ");
	uint32_t x = vga_get_x();
	uint32_t y = vga_get_y();
	printf("%d/%d", total_alloc, total_memory / 4);

	// TODO: Stops working at 1/2 RAM used.
	while (total_alloc < (total_memory / 4)) {
		// Allocate new storage
		uint32_t **new_data = kmalloc(sizeof(void *) * num_alloc);
		memcpy(new_data, allocated_data, sizeof(void *) * (num_alloc - 1));
		kfree(allocated_data);
		allocated_data = new_data;

		allocated_data[num_alloc - 1] = kmalloc(alloc_size);
		*allocated_data[num_alloc - 1] = alloc_size;
		total_alloc += alloc_size;
		alloc_size = MIN(2 * 1024 * 1024, alloc_size * 2);
		num_alloc++;

		vga_set_x(x);
		vga_set_y(y);
		printf("%d/%d", total_alloc, total_memory / 4);
	}

	alloc_size = 1;
	num_alloc = 0;

	printf("\nTesting and Deallocating: ");
	x = vga_get_x();
	y = vga_get_y();
	printf("%d/%d", total_alloc, total_memory / 4);

	while (total_alloc) {
		uint32_t *val = allocated_data[num_alloc];
		if (*val != alloc_size) {
			KPANIC("Bad Value for Index: %d, Expected: %d, Received: %d, Address: %x", num_alloc, alloc_size, *val, val);
		}

		total_alloc -= alloc_size;
		num_alloc++;
		alloc_size = MIN(2 * 1024 * 1024, alloc_size * 2);
		kfree(val);
		vga_set_x(x);
		vga_set_y(y);
		printf("%d/%d", total_alloc, total_memory / 4);
	}

	printf("\n");
}

static void kernel_tick(struct registers *UNUSED(regs)) {
	static uint32_t ticks = 0;

	if (ticks == 10000) {
		timer_done = true;		
	} else {
		vga_set_x(ticks_x);
		vga_set_y(ticks_y);
		printf("%d", ++ticks);
	}
}

static void timer_test() {
	time_x = vga_get_x();
	time_y = vga_get_y();

	printf("Ticks: 0");
	ticks_x = vga_get_x() - 1;
	ticks_y = vga_get_y();

	timer_set_handler(1000, kernel_tick);

	asm volatile ("sti");
	while(!timer_done)
		asm volatile ("hlt");

	// init currently only sets a default callback, so this is adequate
	timer_init();

	printf("\n");
}

static void kernel_keyboard_test() {
	asm volatile ("sti");

	while (true) {
		asm volatile ("hlt");
	}
}

static void thread_task(void *UNUSED(args)) {
	uint32_t cycles = 0;
	uint32_t ticks = 0;

	for (;;) {
		if (ticks++ == 1000) {
			ticks = 0;
			asm volatile ("cli");
		
			uint32_t x = vga_get_x();
			uint32_t y = vga_get_y();
			vga_set_x(68);
			vga_set_y(0);
			rtc_print();
			vga_set_x(x);
			vga_set_y(y);

			asm volatile ("sti");
		}
	}
}

void kernel_main(void) {
	// KLOG("Initializing Keyboard...");
	// keyboard_init();
	// kernel_clock_test();
	// KLOG("Initiating Keyboard Test...");
	// kernel_keyboard_test();
	// printf("Memory Test (%d Bytes): ", (PHYSICAL_MEMORY_END - PHYSICAL_MEMORY_START) / 4);
	// uint32_t x = vga_get_x();
	// uint32_t y = vga_get_y();
	// printf("START\n");
	// memtest();
	// uint32_t tmpx = vga_get_x();
	// uint32_t tmpy = vga_get_y();
	// vga_set_x(x);
	// vga_set_y(y);
	// KLOG("SUCCESS!");
	// vga_set_x(tmpx);
	// vga_set_y(tmpy);

	// printf("SysTimer Test (1KHz): ");
	// x = vga_get_x();
	// y = vga_get_y();
	// printf("START\n");
	// timer_test();
	// tmpx = vga_get_x();
	// tmpy = vga_get_y();
	// vga_set_x(x);
	// vga_set_y(y);
	// KLOG("SUCCESS!");
	// vga_set_x(tmpx);
	// vga_set_y(tmpy);

	KLOG("Initializing Multitasking...");
	proc_init();
	thread_create(thread_task, NULL);

	KLOG("Tests Complete!");
	keyboard_init();
	KLOG("Keyboard Initialized... Press any Button...");

	// Loop infinitely
	while (true) {
		asm volatile ("hlt");
	}
}
