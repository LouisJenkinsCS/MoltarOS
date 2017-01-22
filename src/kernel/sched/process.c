#include <include/sched/process.h>
#include <include/x86/idt.h>
#include <include/mm/alloc.h>
#include <include/drivers/timer.h>
#include <include/kernel/mem.h>
#include <string.h>

static proc_t *head;
static proc_t *current;

// Makes use of an interrupt handler trick, where we manipulate the registers pushed
// on the stack to trick the CPU into jumping into the next process.
static void switch_task(regs_t *regs) {
	// We ensure we make a copy of the current registers pushed on the stack
	// so that they may be restored for later.
	memcpy(&current->regs, regs, sizeof(regs_t));

	// Select the next process
	if (!current->next) {
		current = head;
	} else {
		current = current->next;
	}

	// Restore the new current process' registers
	memcpy(regs, &current->regs, sizeof(regs_t));
}

static void clone_page_directory(uint32_t *parent_dir, uint32_t *child_dir) {
	// Link the first two page directory entries as they belong to the kernel.
	// Remember that each page directory entry is merely a DWORD
	memcpy(child_dir, parent_dir, 8);

	// TODO: Clone the contents of each page directory entry
}

void proc_init() {
	// Create process of ourselves
	proc_t *proc = kmalloc(sizeof(proc_t));
	proc->id = 0;
	proc->next = NULL;

	// Obtain our current page directory
	uint32_t cr3;
	asm volatile ("mov %%cr3, %0" : "=r" (cr3));
	proc->page_directory = (uint32_t *) (cr3 + 0xC0000000);


	timer_set_handler(100, switch_task);
}

int fork() {
	// Ensure we are not interrupted
	asm volatile("cli");

	proc_t *parent = current;
	proc_t *child = kmalloc(sizeof(proc_t));
	child->page_directory = (uint32_t *) alloc_page_directory();

	clone_page_directory(parent->page_directory, child->page_directory);



	// Clone the current directory
	child->id = parent->id + 1;

}