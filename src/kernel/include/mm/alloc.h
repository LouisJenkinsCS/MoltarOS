#ifndef MOLTAROS_ALLOC_H
#define MOLTAROS_ALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define ALLOC_ALIGNED 1 << 0
#define ALLOC_IDENTITY 1 << 1;

typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

void alloc_init();

vaddr_t alloc_block();

vaddr_t alloc_page_directory();

#endif
