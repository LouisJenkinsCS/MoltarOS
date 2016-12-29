#include <include/mm/alloc.h>
#include <include/kernel/logger.h>
#include <include/helpers.h>
#include <string.h>
#include <stdint.h>

const uint32_t PAGE_SIZE = 4 * 1024 * 1024;
const uint32_t NUM_FRAMES = 1024;
static const uint32_t PRESENT = 0x1;
static const uint32_t READ_WRITE = 0x2;
static const uint32_t PAGE_MB = 1 << 7;

// Maximum number of frames is 1024 / 32 = 32, so -1 is a valid error number.
static const uint32_t PAGE_ERR = (uint32_t) -1;

// The current physical memory offset we are allocating in memory. This is a very simple allocator
// and as such only allocates memory, and never frees it, and so this only ever increases.
static uint32_t physical_address_offset;
static uint32_t *page_directory;
static uint32_t alloc_bitmap[NUM_FRAMES / 32];

extern uint32_t PHYSICAL_MEMORY_START;
extern uint32_t PHYSICAL_MEMORY_END;

static uint32_t index_of(uint32_t frame) {
	return frame / 32;
}

static uint32_t first_free_frame() {
	// For each bitmapped frame entry
	for(uint32_t i = 0; i < index_of(NUM_FRAMES); i++) {
		// If all bits are set, then there is nothing here for us
		if (alloc_bitmap[i] != 0xFFFFFFFF) {
			// We know that at least one bit is free, find it
			for (uint32_t j = 0; j < 32; j++) {
				uint32_t bit = 0x1 << j;
				// If the bit to test for is unset, we found our entry
				if (!(alloc_bitmap[i] & bit)) {
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

	KLOG("PDE #%d: addr:%x,present:%d,rw:%d,sz:%d", idx, frame_addr, present, rw, sz);
}

void alloc_init() {
		// Zero necessary fields
		physical_address_offset = 0;
		memset(alloc_bitmap, 0, index_of(NUM_FRAMES) * sizeof(uint32_t));

		// The kernel directory set in page directory neds to get set
		BITMAP_SET(alloc_bitmap, 0);

		
		// Obtain the bootstrap page directory stored in CR3 register.
		uint32_t cr3;
		asm volatile ("mov %%cr3, %0" : "=r" (cr3));
		page_directory = (uint32_t *) (cr3 + 0xC0000000);
		KLOG("Address of Page Directory: %x", cr3);
}

vaddr_t alloc_block() {
	// Obtain the first free frame by cycling through all possible frames for one without it's PRESENT bit set.
	bool found = false;
	for (int i = 0; i < 1024; i++) {
		uint32_t idx = (physical_address_offset / PAGE_SIZE) % NUM_FRAMES;
		if (!(page_directory[idx] & PRESENT)) {
			uint32_t frame_idx = first_free_frame();
			KLOG("Allocation: PDE #%d, Index: %d, Physical Address: %x, Virtual Address: %x", idx, frame_idx, frame_idx * PAGE_SIZE, physical_address_offset);
			
			// Out of Memory
			if (frame_idx == PAGE_ERR) {
				KPANIC("Could not find a free physical address!");
			}

			// Claim the frame
			BITMAP_SET(alloc_bitmap, frame_idx);

			// Mark frame as present and invalidate for TLB
			page_directory[idx] = (frame_idx * PAGE_SIZE) | PAGE_MB | PRESENT | READ_WRITE;
			asm volatile ("invlpg (%0)" :: "b" (physical_address_offset) : "memory");


			debug_pd(idx);
			// Exit early
			found = true;
			break;
		}

		physical_address_offset += PAGE_SIZE;
	}

	// Out of Memory
	if (!found) {
		KPANIC("Could not find a free virtual address!");
	}

	uint32_t retval = physical_address_offset;
	physical_address_offset += PAGE_SIZE;

	// Clear the memory allocated frame for the user
	memset((void *) retval, 0, PAGE_SIZE);
	return retval;
}