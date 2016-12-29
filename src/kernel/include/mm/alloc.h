#ifndef MOLTAROS_ALLOC_H
#define MOLTAROS_ALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

void alloc_init();

vaddr_t alloc_block();

#endif
