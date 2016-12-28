#include <include/mm/alloc.h>
#include <string.h>
#include <stdint.h>

// The current physical memory offset we are allocating in memory. This is a very simple allocator
// and as such only allocates memory, and never frees it, and so this only ever increases.
uint32_t ALLOC_PHYSICAL_ADDRESS_OFFSET;

static const uint32_t PAGE_ALIGNED = 0xFFFFF000;
static const uint32_t STACK_SIZE = 16 * 1024 * 1024;

extern uint32_t PHYSICAL_MEMORY_START;
extern uint32_t PHYSICAL_MEMORY_END;

void alloc_init() {
		ALLOC_PHYSICAL_ADDRESS_OFFSET = PHYSICAL_MEMORY_START + STACK_SIZE;
}

paddr_t alloc(size_t size, uint32_t *phys_addr, bool aligned) {
	// If we need to align it, we need to ensure the pointer is on a 4KB boundary
	if (aligned && (ALLOC_PHYSICAL_ADDRESS_OFFSET & ~PAGE_ALIGNED)) {
		ALLOC_PHYSICAL_ADDRESS_OFFSET &= PAGE_ALIGNED;
		ALLOC_PHYSICAL_ADDRESS_OFFSET += 0x1000;
	}

	// If the user wants the actual physical address...
	if (phys_addr) {
		*phys_addr = ALLOC_PHYSICAL_ADDRESS_OFFSET;
	}

	uint32_t retval = ALLOC_PHYSICAL_ADDRESS_OFFSET;
	ALLOC_PHYSICAL_ADDRESS_OFFSET += size;

	// Clear the memory allocated for the user
	memset((void *) retval, 0, size);
	return retval;
}