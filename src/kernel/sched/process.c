#include <include/sched/process.h>
#include <include/x86/idt.h>
#include <include/mm/alloc.h>
#include <include/drivers/timer.h>
#include <include/kernel/mem.h>
#include <include/kernel/logger.h>
#include <string.h>

#define DUMMY_EIP_STR "0x12345"
#define DUMMY_EIP 0x12345

static proc_t *head;
static proc_t *current;

extern uint32_t read_eip();

extern int STACK_START;
extern int STACK_SIZE;

// Makes use of an interrupt handler trick, where we manipulate the registers pushed
// on the stack to trick the CPU into jumping into the next process.
static void task_switch(regs_t *regs) {
	// KLOG("Preparing to task switch");
	
	proc_t *curr = current;

	// Ensure we preserve our current state so we return here later.
    // Information needing to be saved are the registers esp, ebp, and eip
    uint32_t esp, ebp, eip;
    
    // The stack pointer and base pointer can readily be retrieved
    // but the instruction pointer is a tough one. The instruction pointer
    // MUST be of the instruction following 'read_eip'
    asm volatile ("mov %%esp, %0" : "=r" (esp));
    asm volatile ("mov %%ebp, %0" : "=r" (ebp));
    eip = read_eip();

    // If the instruction pointer is DUMMY_EIP, then we have just jumped here after switching
    // to a new task. Note that the task itself would also have been inside of an interrupt handler, 
    // which forces it to return to it's normal execution.
    if (eip != DUMMY_EIP) {
    	// KLOG("Preserving Current Process Info: pid: %x, eip: %x, esp: %x, ebp: %x", current->id, eip, esp, ebp);
    } else {
    	// KLOG("Child returned, continuing task...");
    	return;
    }

	// Select the next process
	if (!current->next) {
		current = head;
	} else {
		current = current->next;
	}

    curr->eip = eip;
    curr->esp = esp;
    curr->ebp = ebp;

    // Bugged? Need to initiate a function call or else a page fault occurs
   	printf("");
    // KLOG("Task Switch: %d -> %d", curr->id, current->id);
    // KLOG("Jumping to new process... eip: %x, esp: %x, ebp: %x", current->eip, current->esp, current->ebp);

	// Jump to other task. The task's stack pointer and base pointer will accurately point to the
    // task's previous position on the stack.
    asm volatile (
        "mov %0, %%ecx;\n"
        "mov %1, %%esp;\n"
        "mov %2, %%ebp;\n"
        "mov $" DUMMY_EIP_STR ", %%eax;\n"
        "jmp *%%ecx" 
        : : "r" (current->eip), "r" (current->esp), "r" (current->ebp)
    );
}

// Clones the parent's stack for the child. Also ensures that any base pointer no longer
// points to the parent stack.
static void copy_stack(proc_t *child, proc_t *parent) {
	KLOG("Copying stack of %d into %d...", parent->id, child->id);

	uint32_t esp;  asm volatile ("mov %%esp, %0" : "=r" (esp));
	uint32_t ebp;  asm volatile ("mov %%ebp, %0" : "=r" (ebp));
    // Allocate a new page for the child's stack.
    uint32_t new_stack = alloc_block();
    child->stack_start = new_stack;
    KLOG("Allocated new stack: %x", new_stack);
    KLOG("Copying stack %x -> %x, %x Bytes", parent->stack_start, child->stack_start, 0x1000);
    memcpy((uint32_t *) child->stack_start, (uint32_t *) parent->stack_start, 4 * 1024 * 1024);
    KLOG("Copied stack...");
    
    // Any pointers to what is on the parent's stack is invalid, so offset it by the difference between
    // between the parent and child.
    KLOG("Redirecting pointers to other stack...");
    for (uint32_t i = child->stack_start; i < child->stack_start + 0x1000; i += 4) {
        uint32_t addr = *(uint32_t *) i;
        if (addr > parent->stack_start && addr < (parent->stack_start + 0x1000)) {
        	KLOG("Redirecting Pointer %x -> %x", addr, addr + (child->stack_start - parent->stack_start));
            *(uint32_t *) i = addr + (child->stack_start - parent->stack_start); 
        }
    }
}

static void clone_page_directory(uint32_t *parent_dir, uint32_t *child_dir) {
	// Link the first two page directory entries as they belong to the kernel.
	// Remember that each page directory entry is merely a DWORD
	memcpy(child_dir, parent_dir, 8);

	// Link the kernel page directories, page frames 1 and 768

	// Copy other contents by allocating new page frame by virtual address
}

void proc_init() {
	// Create process of ourselves
	proc_t *proc = kmalloc(sizeof(proc_t));
	proc->id = 0;
	proc->stack_start = 0xC0000000 + STACK_START;
	proc->next = NULL;
	current = proc;
	head = proc;
	KLOG("Stack Start: %x", proc->stack_start);

	// Obtain our current page directory
	uint32_t cr3;
	asm volatile ("mov %%cr3, %0" : "=r" (cr3));
	proc->page_directory = (uint32_t *) (cr3 + 0xC0000000);


	timer_set_handler(20, task_switch);
	KLOG("Multitasking initialized...");
}

static proc_t *proc_new() {
	proc_t *proc = kmalloc(sizeof(proc_t));
	memset(proc, 0, sizeof(proc_t));
	KLOG("Created New Process...");
	return proc;
}

int fork() {
	KLOG("Forking...");
	// Ensure we are not interrupted
	asm volatile("cli");

	// Allocate a new process
    proc_t *child = proc_new();
    proc_t *parent = current;
    child->id = parent->id + 1;
    
    // Copy from parent
    copy_stack(child, parent);
    KLOG("Copied stacks...");
    // uint32_t esp;  asm volatile ("mov %%esp, %0" : "=r" (esp));
    // uint32_t ebp;  asm volatile ("mov %%ebp, %0" : "=r" (ebp));
    // for (uint32_t i = ebp & ~0x1000; i <=  ebp; i += 4) {
    //     uint32_t addr = *(uint32_t *) i;
    //     KLOG("%x: %x", i, addr);
    // }
    
    // Append to queue
    child->next = parent->next;
    parent->next = child;
    KLOG("Added child to queue...");
    
    uint32_t eip = read_eip();
    
    // If we are the parent, setup esp/ebp/eip for child
    if (current == parent) {
    	KLOG("Parent setting up child...");
        uint32_t esp, ebp;
        asm volatile ("mov %%esp, %0" : "=r" (esp));
        asm volatile ("mov %%ebp, %0" : "=r" (ebp));
        
        uint32_t offset = child->stack_start - parent->stack_start;

        child->esp = offset + esp;
        child->ebp = offset + ebp;
        child->eip = eip;

        KLOG("Child Process Configured: eip: %x, esp: %x, ebp: %x", child->eip, child->esp, child->ebp);
        
        asm volatile ("sti");

        return child->id;
    } else {
    	

    	// asm volatile ("cli");
    	// for (uint32_t i = current->ebp & ~0x1000; i <= current->ebp; i += 4) {
	    //     uint32_t addr = *(uint32_t *) i;
	    //     KLOG("%x: %x", i, addr);
	    // }

	    // KLOG("Child returning... pid: %d, eip: %x, esp: %x, ebp: %x", current->id, current->eip, current->esp, current->ebp);
    	asm volatile ("sti");
        return 0;
    }

}