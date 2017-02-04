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

static uint32_t *page_directory;
static uint32_t frame_bitmap[NUM_FRAMES / 32];
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

	KTRACE("PDE #%d: addr:%x,present:%d,rw:%d,sz:%d", idx, frame_addr, present, rw, sz);
}

void alloc_init() {
		// Initialize necessary fields.
		memset(frame_bitmap, 0, index_of(NUM_FRAMES) * sizeof(uint32_t));

		// The kernel directory is the first page.
		BITMAP_SET(frame_bitmap, 0);
		BITMAP_SET(frame_bitmap, 1);
		
		// Obtain the bootstrap page directory stored in CR3 register.
		uint32_t cr3;
		asm volatile ("mov %%cr3, %0" : "=r" (cr3));
		page_directory = (uint32_t *) (cr3 + 0xC0000000);
		KTRACE("Address of Page Directory: %x", cr3);
}

// TODO: Take parameter for the current process' page directory table.
vaddr_t alloc_block() {
	// Obtain the first free virtual address by cycling through all PDEs for one without it's PRESENT bit set.
	// This way, even if a process has its own virtual address space, we can easily find an available virtual
	// address based on it's page directory table. Note: We skip the first virtual address page (0x0) so that
	// NULL still becomes an invalid address.
	bool found = false;
	uint32_t virtual_addr = PAGE_SIZE;
	for (int i = 1; i < 1024; i++) {
		if (!(page_directory[i] & PRESENT)) {
			uint32_t frame_idx = first_free_frame(frame_bitmap);
			// KTRACE("Allocation: PDE #%d, Index: %d, Physical Address: %x, Virtual Address: %x", idx, frame_idx, frame_idx * PAGE_SIZE, virtual_addr);
			
			// Out of Memory
			if (frame_idx == PAGE_ERR) {
				KPANIC("Could not find a free physical address!");
			}

			// KTRACE("Idx: %x, Physical Address %x taken for Virtual Address %x", idx, frame_idx * PAGE_SIZE, (char *) virtual_addr);

			// Claim the frame
			BITMAP_SET(frame_bitmap, frame_idx);

			// Mark frame as present and invalidate for TLB
			page_directory[i] = (frame_idx * PAGE_SIZE) | PAGE_MB | PRESENT | READ_WRITE;
			asm volatile ("invlpg %0" :: "m" (virtual_addr));

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

	// Clear the memory allocated frame for the user
	// KTRACE("Clearing chunk %x for user...", retval);
	memset(virtual_addr, 0, PAGE_SIZE);
	return virtual_addr;
}