#ifndef MOLTAROS_BIT_ARRAY_H
#define MOLTAROS_BIT_ARRAY_H

#include <stdint.h>
#include <include/helpers.h>

/*
	Helper macros for implementations of a bit array, normally used in the kernel for keeping arrays of flags.
*/

// Declares a new bit array with the requested size
#define BIT_ARRAY_SZ(idx) CEILING(idx, 8)

#define BIT_ARRAY_SET(arr, idx) (arr[(idx / 8)] |= (1 << (idx % 8)))

#define BIT_ARRAY_GET(arr, idx) (arr[(idx / 8)] & (1 << (idx % 8)))

#define BIT_ARRAY_TST(arr, idx) (BIT_ARRAY_GET(arr, idx) ? 1 : 0)

#define BIT_ARRAY_CLR(arr, idx) (arr[(idx / 8)] &= ~(1 << (idx % 8)))

#endif /* MOLTAROS_BIT_ARRAY_H */