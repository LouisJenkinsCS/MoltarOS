#ifndef MOLTAROS_LOGGER_H
#define MOLTAROS_LOGGER_H

#include <stdio.h>

#ifndef NDEBUG
	#define KLOG(format, ...) printf(format, ##__VA_ARGS__)
#else
	#define KLOG(format, ...)
#endif


// Kernel Panic which will just print error message and spin
#define KPANIC(format, ...) \
do { \
	printf("[%s:%s:%s] PANIC: \"" format "\"", __FILE__, __FUNCTION__, STRINGIFY(__LINE__),  ##__VA_ARGS__); \
	asm volatile ("cli"); \
	while (true) \
		asm volatile ("hlt"); \
} while (0)

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

#endif /* MOLTAROS_LOGGER_H */