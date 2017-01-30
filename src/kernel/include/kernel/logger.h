#ifndef MOLTAROS_LOGGER_H
#define MOLTAROS_LOGGER_H

#include <include/drivers/vga.h>
#include <stdio.h>
#include <stdint.h>

// Our log levels
#define LEVEL_ALL 0
#define LEVEL_VERBOSE 1
#define LEVEL_DEBUG 2
#define LEVEL_INFO 3
#define LEVEL_WARNING 4
#define LEVEL_SEVERE 5

// Current log level. Any below this are filtered
#define LOG_LEVEL LEVEL_INFO

#ifndef NDEBUG
	#define KLOG(level, color, format, ...) \
		do { \
			asm volatile ("cli"); \
			vga_print("["); \
			vga_print_color(color, level); \
			vga_print("] "); \
			printf(format "\n", ##__VA_ARGS__); \
			asm volatile ("sti"); \
		} while (0)
#else
	#define KLOG(level, color, format, ...)
#endif

#if LOG_LEVEL <= LEVEL_INFO
	#define KLOG_INFO(format, ...) KLOG("INFO", COLOR_GREEN, format, ##__VA_ARGS__)
#else
	#define KLOG_INFO(format, ...) printf("")
#endif

// Kernel Panic which will just print error message and spin
#define KPANIC(format, ...) \
do { \
	KLOG("PANIC", COLOR_RED, format, ##__VA_ARGS__); \
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