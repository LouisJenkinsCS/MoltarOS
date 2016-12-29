#ifndef MOLTAROS_MEM_H
#define MOLTAROS_MEM_H

#include <stddef.h>
#include <stdint.h>

extern uint32_t PAGE_SIZE;

void mem_init();

void *kmalloc(size_t sz);

void kfree(void *ptr);

#endif /* endif MOLTAROS_MEM_H */