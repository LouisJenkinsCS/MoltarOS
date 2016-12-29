#include <include/kernel/mem.h>
#include <include/kernel/logger.h>
#include <include/mm/heap.h>
#include <include/mm/alloc.h>

static memheap_t kheap = {0};

static void more_memory() {
	memheap_add_block(&kheap, alloc_block(), PAGE_SIZE, 16);
}

void mem_init() {
	// Initialize modules we depend on
	memheap_init(&kheap);
	alloc_init();

	// Allocate the first chunk of memory for the kernel heap
	more_memory();
}

void *kmalloc(size_t sz) {
	// Validate request
	if (sz > PAGE_SIZE) {
		KPANIC("Bad Allocation Size... Max: %x, Attempt: %x", PAGE_SIZE, sz);
	}

	// Check allocation of heap and add more if need be
	void *data = memheap_alloc(&kheap, sz);
	if (!data) {
		more_memory();
		data = memheap_alloc(&kheap, sz);
	}

	return data;
}

void kfree(void *ptr) {
	memheap_free(&kheap, ptr);
}