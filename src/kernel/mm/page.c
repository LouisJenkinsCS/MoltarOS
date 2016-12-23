#include <include/mm/page.h>
#include <include/mm/alloc.h>
#include <include/helpers.h>
#include <include/idt.h>
#include <stdbool.h>
#include <string.h>

// Helper structure to keep track of global state.
static struct {
	// Mapping of whether a frame is in use or not.
	uint32_t *bitmap;
	// Number of bitmap entries
	uint32_t size;
	// Keep track of the kernel directory in case we switch to another
	page_directory_t *kernel;
	// Our current directory being used
	page_directory_t *current;
} frames;

// While the maximum value for an unsigned integer is equivalent to 4,294,967,296, which
// is the same as the maximum valid memory, we have to take into account that each page
// is 4KBs, and that the number of possible pages is 1,000,000. Hence this is a safe error value.
static const uint32_t PAGE_ERR = (uint32_t) -1;

// Amount of physical memory at our disposal.
extern uint32_t PHYSICAL_MEMORY_SIZE;


static inline uint32_t index_of(uint32_t);

static void page_fault_handler(struct registers *);

static uint32_t first_free_frame();

static void switch_directory(page_directory_t *);

static page_entry_t *get_page(uint32_t, bool, page_directory_t *);



void page_init() {
	// Initialize frames bitmap
	frames.size = PHYSICAL_MEMORY_SIZE / 0x1000;
	frames.bitmap = kcalloc(index_of(frames.size));

	// Create the kernel page directory
	frames.kernel = kcalloc_aligned(sizeof(page_directory_t));
	frames.current = frames.kernel;

	// Fill out directory by allocating pages for all address space up to end of physical memory. 
	// Since the pages are not setup yet, it will be forced to allocate it (sequential to physical address),
	// so virtual address will always be equivalent to physical address.
	for (uint32_t offset = 0; offset < PHYSICAL_MEMORY_SIZE; offset += 0x1000) {
		// Allocates frames that are read-only from userspace  
		page_alloc(get_page(offset, true, frames.kernel), false, false);
	}

	// Register our interrupt handler
	register_interrupt_handler(14, page_fault_handler);

	// Set kernel as current page directory
	switch_directory(frames.kernel);
}

void page_alloc(page_entry_t *entry, bool supervisor, bool write) {
	// If this entry already has the address set, then it has 
	// already been allocated and there is no work to do
	if (entry->frame_addr != 0x0) {
		return;
	}

	uint32_t frame_idx = first_free_frame();
	// Out of Memory	
	if (frame_idx == PAGE_ERR) {
		// TODO: Fix macro, having issues getting to compile
		printf("[%s:%s:%s] PANIC: \"" "Out Of Memory: Failed to locate a free frame!" "\"", __FILE__, __FUNCTION__, STRINGIFY(__LINE__));
		PANIC;
	}
	// Claim the page and setup
	BITMAP_SET(frames.bitmap, frame_idx);
	entry->present = true;
	entry->read_write = write;
	entry->user_mode = !supervisor;
	entry->frame_addr = frame_idx;
}

void page_free(page_entry_t *entry) {
	uint32_t frame_idx = entry->frame_addr;
	// If the frame address is 0, then it isn't allocated
	if (!frame_idx) {
		return;
	}

	// Unmark this frame in bitmap
	BITMAP_CLEAR(frames.bitmap, frame_idx);
	entry->frame_addr = 0x0;
}


static page_entry_t *get_page(uint32_t addr, bool create, page_directory_t *dir) { 
	// Calculations are based on page indice, not raw addresses
	addr /= 0x1000;
	uint32_t table_idx = addr / 1024;
	uint32_t page_idx = addr % 1024;

	// If this table is allocated, obtain the requested address, otherwise create a new one if specified
	if (dir->tables[table_idx]) {
		return &dir->tables[table_idx]->pages[page_idx];
	} else if (create) {
		// Allocate new table and obtain physical address of newly allocated table
		uint32_t physical_addr;
		dir->tables[table_idx] = (page_table_t *) kcalloc_aligned_phys(sizeof(page_table_t), &physical_addr);

		// 0x7 => 0111 => PRESENT | READ-WRITE | USER-MODE
		dir->tables_addr[table_idx] = physical_addr | 0x7;

		return &dir->tables[table_idx]->pages[page_idx];
	} else {
		return NULL;
	}
}

static void switch_directory(page_directory_t *dir) {
	frames.current = dir;
	// Sets the current in-use page tables
	asm volatile ("mov %0, %%cr3" :: "r" (&dir->tables_addr));
	
	// Set register cr0's PG bit to enable paging
	uint32_t cr0;
	asm volatile ("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile ("mov %0, %%cr0" :: "r" (cr0));
}

static uint32_t first_free_frame() {
	// For each bitmapped frame entry
	for(uint32_t i = 0; i < index_of(frames.size); i++) {
		// If all bits are set, then there is nothing here for us
		if (frames.bitmap[i] != 0xFFFFFFFF) {
			// We know that at least one bit is free, find it
			for (uint32_t j = 0; j < 32; j++) {
				uint32_t bit = 0x1 << j;
				// If the bit to test for is unset, we found our entry
				if (!(frames.bitmap[i] & bit)) {
					return i * 32 + j;
				}
			}
		}
	}

	return PAGE_ERR;
}

static const uint32_t NOT_PRESENT = 0x1;
static const uint32_t INVALID_WRITE = 0x2;
static const uint32_t USER_MODE_ACCESS = 0x4;
static const uint32_t RESERVED_OVERWRITTEN = 0x8;
static const uint32_t INSTRUCTION_FETCH = 0x10;

static void page_fault_handler(struct registers *UNUSED(regs)) {
	// Address that triggered the page fault is located in register CR2
	uint32_t fault_addr;
	asm volatile ("mov %%cr2, %0" : "=r" (fault_addr));

	// TODO: Handle page faults
	printf("[%s:%s:%s] PANIC: \"" "Page Fault" "\"", __FILE__, __FUNCTION__, STRINGIFY(__LINE__));
	PANIC;
}

static inline uint32_t index_of(uint32_t idx) {
	return idx / 32;
}
