#ifndef MOLTAROS_LOGGER_H
#define MOLTAROS_LOGGER_H

#include <stdio.h>

#ifndef NDEBUG
	#define KLOG(format, ...) printf(format "\n", ##__VA_ARGS__)
#else
	#define KLOG(format, ...)
#endif


// Kernel Panic which will just print error message and spin
#define KPANIC(format, ...) \
do { \
	printf("[%s:%s:%s] PANIC: \"" format "\"\n", __FILE__, __FUNCTION__, STRINGIFY(__LINE__),  ##__VA_ARGS__); \
	asm volatile ("cli"); \
	while (true) \
		asm volatile ("hlt"); \
} while (0)

#define KFRAME \
do {\
	printf("FUNCTION: %s, FP: %x, CALLER FP: %x\n", __FUNCTION__, __builtin_frame_address(0), __builtin_frame_address(1)); \
} while (0)

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

#endif /* MOLTAROS_LOGGER_H */