#include <include/sched/process.h>
#include <include/x86/idt.h>
#include <include/drivers/timer.h>
#include <include/kernel/mem.h>
#include <string.h>

static proc_t dummy = {};
static proc_t *head = &dummy;
static proc_t *current = &dummy;

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

void proc_init() {
	// We require an initial task so that when we task switch from our original
	// thread, we can properly save it.

	timer_set_handler(100, switch_task);
}

int fork() {
	// Ensure we are not interrupted
	asm volatile("cli");

	proc_t *parent = current;
	proc_t *child = kmalloc(sizeof(proc_t));

	// Clone the current directory
	child->id = parent->id + 1;
	memcpy(&child->regs, &parent->regs, sizeof(regs_t));


}