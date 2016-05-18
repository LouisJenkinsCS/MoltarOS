#ifndef MOLTAROS_GDT_H
#define MOLTAROS_GDT_H

#include <stdint.h>

/*
	Access byte constants used to help with overall readability (and for my own benefit towards
	fully understanding everything) to use when attempting to read and manage these flags.
*/
#define GDT_ACCESS_NONE 0
#define GDT_ACCESS_ACCESSED 1 << 0
#define GDT_ACCESS_RW 1 << 1
#define GDT_ACCESS_DIRECTION 1 << 2
#define GDT_ACCESS_EXECUTABLE 1 << 3
#define GDT_ACCESS_GARBAGE 1 << 4
#define GDT_ACCESS_PRIVILEDGE_RING_ZERO 0 << 5
#define GDT_ACCESS_PRIVILEDGE_RING_ONE 1 << 5
#define GDT_ACCESS_PRIVILEDGE_RING_TWO 2 << 5
#define GDT_ACCESS_PRIVILEDGE_RING_THREE 3 << 5
#define GDT_ACCESS_PRESENT 1 << 7

/*
	Flags byte used to help with overall readability.
*/
#define GDT_FLAGS_NONE 0
#define GDT_FLAGS_GARBAGE 2 << 0
#define GDT_FLAGS_SIZE 1 << 2
#define GDT_FLAGS_GRANULARITY 1 << 3

/*
	Describes an entry in the Global Descriptor Table. The Access and Flags byte is drawn out
	as an individual bitfield to help with readability and reduce the amount of bitwise operations
	needed to manipulate them.
*/
struct __attribute__((packed)) gdt_entry {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	unsigned limit_high : 4;
	unsigned flags : 4;
	uint8_t base_high;
};

/*
	The structure needed by the lgdt x86 instruction which is used to point
	to the GDT structures.
*/
struct __attribute__((packed)) gdt_ptr {
		// The overall size of all gdt_entry's, minus one
		uint16_t limit;
		// The pointer to the first gdt_entry in an array
		uint32_t base;
};



void gdt_init();

#endif /* MOLTAROS_GDT_H */