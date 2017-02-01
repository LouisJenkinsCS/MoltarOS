#include <include/x86/idt.h>
#include <include/x86/exceptions.h>
#include <include/kernel/logger.h>
#include <include/kernel/mem.h>
#include <stdbool.h>

static const uint32_t PRESENT = 0x1;
static const uint32_t READ_WRITE = 0x2;
static const uint32_t USER_MODE = 0x4;
static const uint32_t RESERVED = 0x8;
static const uint32_t INSTRUCTION_FETCH = 0x16;

static void page_fault_handler(struct registers *r) {
	// Address that triggered the page fault is located in register CR2
	uint32_t fault_addr;
	asm volatile ("mov %%cr2, %0" : "=r" (fault_addr));

	const char *msg = NULL;
	switch (r->err_code & (PRESENT | READ_WRITE | USER_MODE)) {
		case 0:
			msg = "Supervisory process tried to read a non-present page entry";
			break;
		case PRESENT:
			msg = "Supervisory process tried to read a page and caused a protection fault";
			break;
		case READ_WRITE:
			msg = "Supervisory process tried to write to a non-present page entry";
			break;
		case PRESENT | READ_WRITE:
			msg = "Supervisory process tried to write a page and caused a protection fault";
			break;
		case USER_MODE:
			msg = "User process tried to read a non-present page entry";
			break;
		case USER_MODE | PRESENT:
			msg = "User process tried to read a page and caused a protection fault";
			break;
		case USER_MODE | READ_WRITE:
			msg = "User process tried to write to a non-present page entry";
			break;
		case USER_MODE | READ_WRITE | PRESENT:
			msg = "User process tried to write a page and caused a protection fault";
			break;
	}

	const char *extra = NULL;
	switch(r->err_code & (INSTRUCTION_FETCH | RESERVED)) {
		case 0:
			extra = "Page Fault was not caused by an instruction fetch or overwrite of reserved bits";
			break;
		case INSTRUCTION_FETCH:
			extra = "Page Fault was caused by an instruction fetch";
			break;
		case RESERVED:
			extra = "Page Fault was caused by an overwrite of reserved bits";
			break;
		case INSTRUCTION_FETCH | RESERVED:
			extra = "Page Fault was caused by both an instruction fetch and an overwrite of reserved bits";
			break;
	}

	KPANIC("Segmentation Fault... Exception: \"Page Fault at Address: 0x%x\" \nReason: \"%s\" \nDebug: \"%s\"" 
		"\nRegisters{ebp=0x%x, esp=0x%x, eip=0x%x, eflags=0x%x, user_esp=0x%x}", 
		fault_addr, msg, extra, r->ebp, r->esp, r->eip, r->eflags, r->useresp);
}

void exceptions_init() {
	register_interrupt_handler(14, page_fault_handler);
}