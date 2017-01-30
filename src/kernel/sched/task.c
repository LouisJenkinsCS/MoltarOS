#include <include/sched/task.h>
#include <include/x86/idt.h>
#include <include/mm/alloc.h>
#include <include/helpers.h>
#include <include/drivers/timer.h>
#include <include/kernel/mem.h>
#include <include/kernel/logger.h>
#include <include/ds/list.h>
#include <string.h>

#define DUMMY_EIP_STR "0x12345"
#define DUMMY_EIP 0x12345

static list_t *tasks;

extern uint32_t read_eip();

extern int STACK_START;
extern int STACK_SIZE;

// Makes use of an interrupt handler trick, where we manipulate the registers pushed
// on the stack to trick the CPU into jumping into the next process.
static void task_switch(regs_t *UNUSED(regs)) {
	// KLOG_INFO("Preparing to task switch");
	
	task_t *curr = list_current(tasks);

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
    	// KLOG_INFO("Preserving Current Process Info: pid: %x, eip: %x, esp: %x, ebp: %x", current->id, eip, esp, ebp);
    } else {
    	// KLOG_INFO("Child returned, continuing task...");
    	return;
    }

	// Select the next process
	task_t *next = list_next(tasks);
	if (!next) {
		next = list_head(tasks);
	}

	// Preserve the current task's progress.
    curr->eip = eip;
    curr->esp = esp;
    curr->ebp = ebp;

    // Bugged? Need to initiate a function call or else a page fault occurs
   	printf("");
    // KLOG_INFO("Task Switch: %d -> %d", curr->id, current->id);
    // KLOG_INFO("Jumping to new process... eip: %x, esp: %x, ebp: %x", current->eip, current->esp, current->ebp);

	// Jump to other task. The task's stack pointer and base pointer will accurately point to the
    // task's previous position on the stack.
    asm volatile (
        "mov %0, %%ecx;\n"
        "mov %1, %%esp;\n"
        "mov %2, %%ebp;\n"
        "mov $" DUMMY_EIP_STR ", %%eax;\n"
        "jmp *%%ecx" 
        : : "r" (next->eip), "r" (next->esp), "r" (next->ebp)
    );
}

// Clones the parent's stack for the child. Also ensures that any base pointer no longer
// points to the parent stack.
static void copy_stack(task_t *child, task_t *parent) {
	KLOG_INFO("Copying stack of %d into %d...", parent->id, child->id);

	uint32_t esp;  asm volatile ("mov %%esp, %0" : "=r" (esp));
	uint32_t ebp;  asm volatile ("mov %%ebp, %0" : "=r" (ebp));
    // Allocate a new page for the child's stack.
    uint32_t new_stack = alloc_block();
    child->stack_start = new_stack;
    KLOG_INFO("Allocated new stack: %x", new_stack);
    KLOG_INFO("Copying stack %x -> %x, %x Bytes", parent->stack_start, child->stack_start, 0x1000);
    memcpy((uint32_t *) child->stack_start, (uint32_t *) parent->stack_start, 4 * 1024 * 1024);
    KLOG_INFO("Copied stack...");
    
    // Any pointers to what is on the parent's stack is invalid, so offset it by the difference between
    // between the parent and child.
    KLOG_INFO("Redirecting pointers to other stack...");
    for (uint32_t i = child->stack_start; i < child->stack_start + 0x1000; i += 4) {
        uint32_t addr = *(uint32_t *) i;
        if (addr > parent->stack_start && addr < (parent->stack_start + 0x1000)) {
        	KLOG_INFO("Redirecting Pointer %x -> %x", addr, addr + (child->stack_start - parent->stack_start));
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

void task_init() {
	tasks = list_create();
	// Create process of ourselves
	task_t *task = kmalloc(sizeof(task_t));
	task->id = 0;
	task->eip = 0;
	task->esp = 0;
	task->ebp = 0;
	task->stack_start = 0xC0000000 + STACK_START;
	list_append(tasks, task);

	KLOG_INFO("Stack Start: %x", task->stack_start);

	// // Obtain our current page directory
	// uint32_t cr3;
	// asm volatile ("mov %%cr3, %0" : "=r" (cr3));
	// task->page_directory = (uint32_t *) (cr3 + 0xC0000000);


	timer_set_handler(20, task_switch);
	KLOG_INFO("Multitasking initialized...");
}

static task_t *task_new() {
	task_t *task = kmalloc(sizeof(task_t));
	memset(task, 0, sizeof(task_t));
	KLOG_INFO("Created New Process...");
	return task;
}

void thread_create(void (*task)(void *args), void *args) {
	KLOG_INFO("Forking...");
	// Ensure we are not interrupted
	asm volatile("cli");

	// Allocate a new process
    task_t *child = task_new();
    task_t *parent = list_current(tasks);
    child->id = parent->id + 1;
    
    // Copy from parent
    copy_stack(child, parent);
    KLOG_INFO("Copied stacks...");
    
    // Append to queue
    list_front(tasks, child);
    KLOG_INFO("Added child to queue...");
    
    uint32_t eip = read_eip();
    
    // If we are the parent, setup esp/ebp/eip for child
    if (list_current(tasks) == parent) {
    	KLOG_INFO("Parent setting up child...");
        uint32_t esp, ebp;
        asm volatile ("mov %%esp, %0" : "=r" (esp));
        asm volatile ("mov %%ebp, %0" : "=r" (ebp));
        
        uint32_t offset = child->stack_start - parent->stack_start;

        child->esp = offset + esp;
        child->ebp = offset + ebp;
        child->eip = eip;

        KLOG_INFO("Child Process Configured: eip: %x, esp: %x, ebp: %x", child->eip, child->esp, child->ebp);
        
        asm volatile ("sti");
    } else {
    	// Inside of child, so execute the thread. If we exit early, it is an error as we do not have a way to handle this.
    	task(args);

    	KPANIC("Thread returned early! Currently no implemented way to return allocated stack!");
	    // KLOG_INFO("Child returning... pid: %d, eip: %x, esp: %x, ebp: %x", current->id, current->eip, current->esp, current->ebp);
    }

}