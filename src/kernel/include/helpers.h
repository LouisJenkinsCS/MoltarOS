#ifndef MOLTAROS_HELPERS_H
#define MOLTAROS_HELPERS_H

#include <stdio.h>
#include <stdbool.h>

// Ceiling of integer divison.
#define CEILING(x,y) (((x) + (y) - 1) / (y))

// Get rid of annoying compiler warnings
#define UNUSED(x) (__attribute__((__unused__)) x)

/*
	Helper macros for implementations of a bit array, normally used in the kernel for keeping arrays of flags.
*/

// Declares a new bit array with the requested size
#define BITMAP_SIZE(idx) CEILING(idx, 32)

// Set bit
#define BITMAP_SET(arr, idx) (arr[(idx / 32)] |= (1 << (idx % 32)))

// Get raw value of bit
#define BITMAP_GET(arr, idx) (arr[(idx / 32)] & (1 << (idx % 32)))

// Get value as 0 (false) or 1 (true)
#define BITMAP_TEST(arr, idx) (!!BIT_ARRAY_GET(arr, idx))

// Clear bit
#define BITMAP_CLEAR(arr, idx) (arr[(idx / 32)] &= ~(1 << (idx % 32)))

#endif /* end MLTAROS_HELPERS_H */