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

extern void perform_task_switch(uint32_t, uint32_t, uint32_t);

extern uint32_t read_eip();

extern uint32_t STACK_START;
static uint32_t STACK_SIZE = 0x1000;

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
   	// printf("");
    // KLOG_INFO("Task Switch: %d -> %d", curr->id, current->id);
    // KLOG_INFO("Jumping to new process... eip: %x, esp: %x, ebp: %x", current->eip, current->esp, current->ebp);

	// Jump to other task. The task's stack pointer and base pointer will accurately point to the
    // task's previous position on the stack.
    perform_task_switch(next->eip, next->ebp, next->esp);
    // asm volatile (
    //     "mov %0, %%ecx;\n"
    //     "mov %1, %%esp;\n"
    //     "mov %2, %%ebp;\n"
    //     "mov $" DUMMY_EIP_STR ", %%eax;\n"
    //     "jmp *%%ecx" 
    //     : : "r" (next->eip), "r" (next->esp), "r" (next->ebp)
    // );
}

// Clones the parent's stack for the child task, ensuring that it can start in a safe
// and consistent state. All slots in the stack are scanned to determine if they are
// pointers to the other's stack, and are redirected to the new one. This is so that
// all frame pointers are corrected.
static void copy_stack(task_t *child, task_t *parent) {
	KLOG_INFO("Copying stack of %d at %x into %d...", parent->id, parent->stack_start, child->id);

    // Allocate a new page for the child's stack.
    uint32_t new_stack = alloc_block();
    KLOG_INFO("Allocated block: %x", new_stack);
    KLOG_INFO("Copying stack from %x -> %x", parent->stack_start, new_stack);
    uint32_t esp;  asm volatile ("mov %%esp, %0" : "=r" (esp));
    uint32_t offset = new_stack + PAGE_SIZE - (parent->stack_start + PAGE_SIZE - esp) ;
    // TODO: Do not scan pass stack pointer!!!
    for (uint32_t addr = esp; addr < parent->stack_start + PAGE_SIZE; addr += 4, offset += 4) {
        uint32_t *word = (uint32_t *) addr;
        if (*word < (parent->stack_start + PAGE_SIZE) && *word > parent->stack_start) {
            uint32_t diff = *word - parent->stack_start;
            KLOG_INFO("Redirecting Pointer in stack at %x with value %x -> %x", word, *word, new_stack + diff);
            * (uint32_t *) offset = new_stack + diff; 
        } else {
            * (uint32_t *) offset = *word;
        }
    }

    KLOG_INFO("Finished copying stack of %d", child->id);
    child->stack_start = new_stack;
}

static void clone_page_directory(uint32_t *parent_dir, uint32_t *child_dir) {
	// Link the first two page directory entries as they belong to the kernel.
	// Remember that each page directory entry is merely a DWORD
	memcpy(child_dir, parent_dir, 8);

	// Link the kernel page directories, page frames 1 and 768

	// Copy other contents by allocating new page frame by virtual address
}

// Moves the kernel's stack to a larger one (4KB -> 4MB).
// All slots in the stack are scanned to determine if they are
// pointers to the other's stack, and are redirected to the new one. This is so that
// all frame pointers are corrected.
static uint32_t move_stack() {
    // The stack reserved for the kernel is exactly a page size (4MB) after the first reserved page.
    uint32_t new_stack = PAGE_SIZE + 0xC0000000;
    KLOG_INFO("Allocated block: %x", new_stack);
    KLOG_INFO("Moving kernel stack from %x -> %x", STACK_START, new_stack);

    uint32_t esp;  asm volatile ("mov %%esp, %0" : "=r" (esp));
    uint32_t offset = new_stack + PAGE_SIZE - (STACK_START - esp);
    KLOG_INFO("Redirecting pointers from old stack to new stack. Stack Size: %x...", (STACK_START - esp));
    
    // Scan all slots from the stack pointer up to the beginning of the stack for potential
    // pointers, and redirect them.
    for (uint32_t addr = esp; addr < STACK_START; addr += 4, offset += 4) {
        uint32_t *word = (uint32_t *) addr;
        if (*word < STACK_START && *word > esp) {
            uint32_t diff = *word - esp;
            KLOG_INFO("Redirecting Pointer %x -> %x", *word, new_stack + diff);
            * (uint32_t *) offset = new_stack + diff; 
        } else {
            * (uint32_t *) offset = *word;
        }
    }

    KLOG_INFO("Switching to new stack...");
    
    // The new stack pointer and base pointer are relative to the new allocated stack.
    uint32_t old_esp, old_ebp, new_esp, new_ebp;
    asm volatile ("mov %%esp, %0" : "=r" (old_esp));
    asm volatile ("mov %%ebp, %0" : "=r" (old_ebp));
    new_esp = new_stack + PAGE_SIZE - (STACK_START - old_esp);
    new_ebp = new_stack + PAGE_SIZE - (STACK_START - old_ebp);

    KLOG_INFO("ESP: %x -> %x;EBP: %x -> %x", old_esp, new_esp, old_ebp, new_ebp);

    // Switch to the new kernel stack. We don't bother cleaning up the old one, as it isn't normally less than 1 KB.
    asm volatile ("cli");
    asm volatile (
        "mov %0, %%esp;\n"
        "mov %1, %%ebp;\n"
        :: "r" (new_esp), "r" (new_ebp)
    );
    asm volatile ("sti");

    KLOG_INFO("Moved stack, returning...");

    return new_stack;
}

void task_init() {
    // Move ourselves to a larger stack (4MB in size)
    uint32_t stack = move_stack();
    KLOG_INFO("Moved stack successfully...");

	tasks = list_create();
	// Create process of ourselves
	task_t *task = kmalloc(sizeof(task_t));
	task->id = 0;
	task->eip = 0;
	task->esp = 0;
	task->ebp = 0;
	task->stack_start = stack;
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