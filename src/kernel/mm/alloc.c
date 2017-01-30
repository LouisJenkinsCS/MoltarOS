#include <include/mm/alloc.h>
#include <include/kernel/logger.h>
#include <include/helpers.h>
#include <string.h>
#include <stdint.h>

const uint32_t PAGE_SIZE = 4 * 1024 * 1024;
const uint32_t CHUNK_SIZE = 4 * 1024;
const uint32_t NUM_FRAMES = 1024;
static const uint32_t PRESENT = 0x1;
static const uint32_t READ_WRITE = 0x2;
static const uint32_t PAGE_MB = 1 << 7;

// Maximum number of frames is 1024 / 32 = 32, so -1 is a valid error number.
static const uint32_t PAGE_ERR = (uint32_t) -1;

// The current physical memory offset we are allocating in memory. This is a very simple allocator
// and as such only allocates memory, and never frees it, and so this only ever increases.
static uint32_t virtual_addr;
static uint32_t *page_directory;
static uint32_t alloc_bitmap[NUM_FRAMES / 32];
static uint32_t dir_bitmap[NUM_FRAMES / 32];

extern uint32_t PHYSICAL_MEMORY_START;
extern uint32_t PHYSICAL_MEMORY_END;

static uint32_t index_of(uint32_t frame) {
	return frame / 32;
}

static uint32_t first_free_frame(uint32_t *bitmap) {
	// For each bitmapped frame entry
	for(uint32_t i = 0; i < index_of(NUM_FRAMES); i++) {
		// If all bits are set, then there is nothing here for us
		if (bitmap[i] != 0xFFFFFFFF) {
			// We know that at least one bit is free, find it
			for (uint32_t j = 0; j < 32; j++) {
				uint32_t bit = 0x1 << j;
				// If the bit to test for is unset, we found our entry
				if (!(bitmap[i] & bit)) {
					return i * 32 + j;
				}
			}
		}
	}

	return PAGE_ERR;
}

static void debug_pd(uint32_t idx) {
	uint32_t cr3;
	asm volatile ("mov %%cr3, %0" : "=r" (cr3));
	uint32_t *pd = (uint32_t *) (cr3 + 0xC0000000);
	uint32_t pde = pd[idx];

	uint32_t frame_addr = (pde & 0xFFFFF000);
	bool present = pde & PRESENT;
	bool rw = pde & READ_WRITE;
	bool sz = pde & PAGE_MB;

	KLOG_INFO("PDE #%d: addr:%x,present:%d,rw:%d,sz:%d", idx, frame_addr, present, rw, sz);
}

void alloc_init() {
		// Initialize necessary fields. We also start after the first page because the address
		// 0x0 is commonly used for NULL, and after the second because it was reserved for us.
		virtual_addr = PAGE_SIZE;
		memset(alloc_bitmap, 0, index_of(NUM_FRAMES) * sizeof(uint32_t));

		// The kernel directory and the reserved memory for our usage are the first two
		// pages in memory, so we need to also reserve them for our bitmap.
		BITMAP_SET(alloc_bitmap, 0);
		BITMAP_SET(alloc_bitmap, 1);

		
		// Obtain the bootstrap page directory stored in CR3 register.
		uint32_t cr3;
		asm volatile ("mov %%cr3, %0" : "=r" (cr3));
		page_directory = (uint32_t *) (cr3 + 0xC0000000);
		KLOG_INFO("Address of Page Directory: %x", cr3);
}

// Allocates a page directory, using one of the free identity mapped reserved 4KB frames
vaddr_t alloc_page_directory() {
	// Obtain the first free 4KB sized and aligned chunk to be used as the page directory
	uint32_t dir_idx = first_free_frame(dir_bitmap);

	// Out of Memory
	if (dir_idx == PAGE_ERR) {
		KPANIC("Could not find a free chunk for page directory!");
	}

	// Claim the frame
	BITMAP_SET(dir_bitmap, dir_idx);

	// Obtain and clear the chunk for use
	vaddr_t dir = PAGE_SIZE + (dir_idx * CHUNK_SIZE);
	memset((uint8_t *) dir, 0, CHUNK_SIZE);

	return dir;
}

vaddr_t alloc_block() {
	// Obtain the first free frame by cycling through all possible frames for one without it's PRESENT bit set.
	bool found = false;
	for (int i = 0; i < 1024; i++) {
		uint32_t idx = (virtual_addr / PAGE_SIZE) % NUM_FRAMES;
		if (!(page_directory[idx] & PRESENT)) {
			uint32_t frame_idx = first_free_frame(alloc_bitmap);
			// KLOG_INFO("Allocation: PDE #%d, Index: %d, Physical Address: %x, Virtual Address: %x", idx, frame_idx, frame_idx * PAGE_SIZE, virtual_addr);
			
			// Out of Memory
			if (frame_idx == PAGE_ERR) {
				KPANIC("Could not find a free physical address!");
			}

			// Claim the frame
			BITMAP_SET(alloc_bitmap, frame_idx);

			// Mark frame as present and invalidate for TLB
			page_directory[idx] = (frame_idx * PAGE_SIZE) | PAGE_MB | PRESENT | READ_WRITE;
			asm volatile ("invlpg (%0)" :: "b" (virtual_addr) : "memory");


			// debug_pd(idx);
			// Exit early
			found = true;
			break;
		}

		virtual_addr += PAGE_SIZE;
	}

	// Out of Memory
	if (!found) {
		KPANIC("Could not find a free virtual address!");
	}

	uint32_t retval = virtual_addr;
	virtual_addr += PAGE_SIZE;

	// Clear the memory allocated frame for the user
	memset((void *) retval, 0, PAGE_SIZE);
	return retval;
}