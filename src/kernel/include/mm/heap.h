#ifndef MOLTAROS_MEMORY_MANAGEMENT_H
#define MOLTAROS_MEMORY_MANAGEMENT_H

#include <stdint.h>
#include <stddef.h>

typedef struct memblock memblock_t;
typedef struct memheap memheap_t;

// memblock_t is a simple memory "superblock", which is effectively a header to
// a chunk of blocks of memory: it should not be confused with a block in and of itself.
// It is the header for a contiguous chunk of memory, given by the user. It also uses some
// space to hold a bitmap, acting as a descriptor for certain blocks.
// Implementation based on Pancakes' Bitmap Heap, seen here: http://wiki.osdev.org/User:Pancakes/BitmapHeapImplementation
struct memblock {
	// Pointer to the next superblock in a linked-list fashion.
	memblock_t *next;
	// The total size in raw bytes of this superblock.
	uint32_t total_size;
	// Number of blocks used.
	uint32_t used;
	// The size of the individual blocks of memory.
	uint32_t block_size;
	// Index to the bitmap entry after the last allocation, a naive optimization where
	// the next allocation is assumed to be more likely in the same superblock and contain
	// free space immediately after. This yields significant increase in performance when the
	// block has not filled the initial superblock, but may degrade performance when it has.
	uint32_t last_alloc;
};

struct memheap {
	memblock_t *head;
};

void memheap_init(memheap_t *heap);

void memheap_add_block(memheap_t *heap, uintptr_t addr, uint32_t size, uint32_t block_size);

void *memheap_alloc(memheap_t *heap, uint32_t size);

void memheap_free(memheap_t *heap, void *ptr);

#endif /* endif MOLTAROS_MEMORY_MANAGEMENT_H */