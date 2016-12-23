#ifndef MOLTAROS_HELPERS_H
#define MOLTAROS_HELPERS_H

#include <stdio.h>
#include <stdbool.h>

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

// Kernel Panic which will just print error message and spin
#define PANIC \
do { \
	asm volatile ("cli"); \
	while (true) \
		asm volatile ("hlt"); \
} while (0)

#define PANIC_MSG(format, ...) (printf("[%s:%s:%s] PANIC: \"" format "\"", __FILE__, __FUNCTION__, STRINGIFY(__LINE__),  __VA_ARGS__))


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