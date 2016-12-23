#ifndef MOLTAROS_ALLOC_H
#define MOLTAROS_ALLOC_H

#include <stddef.h>
#include <stdint.h>

void *kmalloc(size_t size);

void *kcalloc(size_t size);

void *kmalloc_aligned(size_t size);

void *kcalloc_aligned_phys(size_t size, uint32_t *phys_ptr);

void *kcalloc_aligned(size_t size);

void kfree(void *ptr);

#endif
