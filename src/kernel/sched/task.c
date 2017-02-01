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

// Needed for determining the initial stack start offset, so we know how much to copy over.
extern uint32_t STACK_START;

// To correct an extremely fatal bug where GCC's inline assembly will automatically reuse
// certain registers without caring if it has been assigned to, we use a dedicated assembly
// procedure to handle task switching for us. The bug originally caused a 'mov' instruction
// of the instruction pointer into the 'ecx' register to be reused in the 'mov' instruction
// into the 'esp' (stack pointer). Since it assumed that the 'ecx' register was reserved, it
// also stored the original stack pointer there as well, so our instruction pointer which overwrote
// the original contents, was being pushed into 'esp'. In summary: It caused the stack to point
// to the code segment (possible since we use a flat memory model), and subsequent stack operations
// would unfortunately cause the instruction pointer to point to invalid instructions. Tragic...
extern void perform_task_switch(uint32_t, uint32_t, uint32_t);

// Assembly function to return the current instruction pointer. The instruction pointer is used
// to jump to when we clone our current task/process, so we effectively 'return twice'.
extern uint32_t read_eip();


// The list of tasks. Since we are currently a uniprocessor system, there is no need to worry
// about concurrent access, however, all accesses to it should disable interrupts.
static list_t *tasks;

// Moves the kernel's stack to a larger one (4KB -> 4MB).
// All slots in the stack are scanned to determine if they are
// pointers to the other's stack, and are redirected to the new one. This is so that
// all frame pointers are corrected.
static uint32_t move_stack();

// Clones the parent's stack for the child task, ensuring that it can start in a safe
// and consistent state. All slots in the stack are scanned to determine if they are
// pointers to the other's stack, and are redirected to the new one. This is so that
// all frame pointers are corrected.
static void copy_stack(task_t *child, task_t *parent);

// Makes use of an interrupt handler trick, where we manipulate the registers pushed
// on the stack to trick the CPU into jumping into the next process.
static void task_switch(regs_t *regs);

// Helper to create a new task structure
static task_t *task_new();

void task_init() {
    // Move ourselves to a larger stack (4MB in size)
    uint32_t stack = move_stack();
    KTRACE("Moved stack successfully...");

	// Create process of ourselves
	task_t *task = kmalloc(sizeof(task_t));
	task->id = 0;
	task->eip = 0;
	task->esp = 0;
	task->ebp = 0;
	task->stack_start = stack;
	
    tasks = list_create();
    list_append(tasks, task);

	KTRACE("Stack Start: %x", task->stack_start);

	timer_set_handler(20, task_switch);
	KTRACE("Multitasking initialized...");
}

// TODO: Don't bother copying thread stack and just initialize a new one.
void thread_create(void (*task)(void *args), void *args) {
	KTRACE("Forking...");
	// Ensure we are not interrupted
	asm volatile("cli");

	// Allocate a new process
    task_t *child = task_new();
    task_t *parent = list_current(tasks);
    child->id = parent->id + 1;
    
    // Copy from parent
    copy_stack(child, parent);
    KTRACE("Copied stacks...");
    
    // Append to queue
    list_front(tasks, child);
    KTRACE("Added child to queue...");
    
    uint32_t eip = read_eip();
    
    // If we are the parent, setup esp/ebp/eip for child
    if (list_current(tasks) == parent) {
    	KTRACE("Parent setting up child...");
        uint32_t esp, ebp;
        asm volatile ("mov %%esp, %0" : "=r" (esp));
        asm volatile ("mov %%ebp, %0" : "=r" (ebp));
        
        uint32_t offset = child->stack_start - parent->stack_start;

        child->esp = offset + esp;
        child->ebp = offset + ebp;
        child->eip = eip;

        KTRACE("Child Process Configured: eip: %x, esp: %x, ebp: %x", child->eip, child->esp, child->ebp);
        
        asm volatile ("sti");
    } else {
    	// Inside of child, so execute the thread. If we exit early, it is an error as we do not have a way to handle this.
    	task(args);

    	KPANIC("Thread returned early! Currently no implemented way to return allocated stack!");
    }
}

static task_t *task_new() {
    task_t *task = kmalloc(sizeof(task_t));
    memset(task, 0, sizeof(task_t));
    KTRACE("Created New Process...");
    return task;
}

static void task_switch(regs_t *UNUSED(regs)) {
    // KTRACE("Preparing to task switch");
    
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
        // KTRACE("Preserving Current Process Info: pid: %x, eip: %x, esp: %x, ebp: %x", current->id, eip, esp, ebp);
    } else {
        // KTRACE("Child returned, continuing task...");
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
    // KTRACE("Task Switch: %d -> %d", curr->id, current->id);
    // KTRACE("Jumping to new process... eip: %x, esp: %x, ebp: %x", current->eip, current->esp, current->ebp);

    // Jump to other task. The task's stack pointer and base pointer will accurately point to the
    // task's previous position on the stack.
    perform_task_switch(next->eip, next->ebp, next->esp);
}

static void copy_stack(task_t *child, task_t *parent) {
    KTRACE("Copying stack of %d at %x into %d...", parent->id, parent->stack_start, child->id);

    // Allocate a new page for the child's stack.
    uint32_t new_stack = alloc_block();
    KTRACE("Allocated block: %x", new_stack);
    KTRACE("Copying stack from %x -> %x", parent->stack_start, new_stack);

    uint32_t esp;  asm volatile ("mov %%esp, %0" : "=r" (esp));
    uint32_t offset = new_stack + PAGE_SIZE - (parent->stack_start + PAGE_SIZE - esp) ;

    // Copy over stack from stack pointer up.
    for (uint32_t addr = esp; addr < parent->stack_start + PAGE_SIZE; addr += 4, offset += 4) {
        uint32_t *word = (uint32_t *) addr;
        if (*word < (parent->stack_start + PAGE_SIZE) && *word > parent->stack_start) {
            uint32_t diff = *word - parent->stack_start;
            KTRACE("Redirecting Pointer in stack at %x with value %x -> %x", word, *word, new_stack + diff);
            * (uint32_t *) offset = new_stack + diff; 
        } else {
            * (uint32_t *) offset = *word;
        }
    }

    KTRACE("Finished copying stack of %d", child->id);
    child->stack_start = new_stack;
}


static uint32_t move_stack() {
    // The stack reserved for the kernel is exactly a page size (4MB) after the first reserved page.
    uint32_t new_stack = PAGE_SIZE + 0xC0000000;
    KTRACE("Allocated block: %x", new_stack);
    KTRACE("Moving kernel stack from %x -> %x", STACK_START, new_stack);

    uint32_t esp;  asm volatile ("mov %%esp, %0" : "=r" (esp));
    uint32_t offset = new_stack + PAGE_SIZE - (STACK_START - esp);
    KTRACE("Redirecting pointers from old stack to new stack. Stack Size: %x...", (STACK_START - esp));
    
    // Scan all slots from the stack pointer up to the beginning of the stack for potential
    // pointers, and redirect them.
    for (uint32_t addr = esp; addr < STACK_START; addr += 4, offset += 4) {
        uint32_t *word = (uint32_t *) addr;
        if (*word < STACK_START && *word > esp) {
            uint32_t diff = *word - esp;
            KTRACE("Redirecting Pointer %x -> %x", *word, new_stack + diff);
            * (uint32_t *) offset = new_stack + diff; 
        } else {
            * (uint32_t *) offset = *word;
        }
    }

    KTRACE("Switching to new stack...");
    
    // The new stack pointer and base pointer are relative to the new allocated stack.
    uint32_t old_esp, old_ebp, new_esp, new_ebp;
    asm volatile ("mov %%esp, %0" : "=r" (old_esp));
    asm volatile ("mov %%ebp, %0" : "=r" (old_ebp));
    new_esp = new_stack + PAGE_SIZE - (STACK_START - old_esp);
    new_ebp = new_stack + PAGE_SIZE - (STACK_START - old_ebp);

    KTRACE("ESP: %x -> %x;EBP: %x -> %x", old_esp, new_esp, old_ebp, new_ebp);

    // Switch to the new kernel stack. We don't bother cleaning up the old one, as it isn't normally less than 1 KB.
    asm volatile ("cli");
    asm volatile (
        "mov %0, %%esp;\n"
        "mov %1, %%ebp;\n"
        :: "r" (new_esp), "r" (new_ebp)
    );
    asm volatile ("sti");

    return new_stack;
}