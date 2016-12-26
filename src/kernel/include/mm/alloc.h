#ifndef MOLTAROS_ALLOC_H
#define MOLTAROS_ALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

void alloc_init();

paddr_t alloc(size_t size, uint32_t *phys_addr, bool aligned);

#endif
