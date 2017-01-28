#ifndef MOLTAROS_LOGGER_H
#define MOLTAROS_LOGGER_H

#include <stdio.h>
#include <stdint.h>

#ifndef NDEBUG
	#define KLOG(format, ...) printf(format "\n", ##__VA_ARGS__)
#else
	#define KLOG(format, ...)
#endif

#define KSTACK(nFrames, firstParam) \
do { \
	uin32_t *ebp = &firstParam - 2; \
	KLOG("Stack Trace:"); \
	for (uin32_t frame = 0; frame < nFrames; frame++) { \
		uin32_t eip = ebp[1]; \
		if (!eip) { \
			break; \
		} \
		\
		ebp = (uin32_t *) ebp[0]; \
		uin32_t *args = &ebp[2]; \
		LOG("eip: %x", eip); \
	} \
}

static void debug_stack(uint32_t nFrames) {
	uint32_t *ebp = &nFrames - 2;
	KLOG("Stack Trace:");
	for (uint32_t frame = 0; frame < nFrames; frame++) {
		uint32_t eip = ebp[1];
		if (!eip) {
			break;
		}
		KLOG("eip: %x, ebp: %x", eip, ebp);
		ebp = (uint32_t *) ebp[0];	
	}
	KLOG("End Stack Trace...");
}


// Kernel Panic which will just print error message and spin
#define KPANIC(format, ...) \
do { \
	printf("\n[%s:%s:%s] PANIC: \"" format "\"\n", __FILE__, __FUNCTION__, STRINGIFY(__LINE__),  ##__VA_ARGS__); \
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