#include <include/kernel/gdt.h>

#include <stdint.h>

/*
	There are only three GDT entries we care about right now: The code and data segments, and the
	null descriptor. The null descriptor is a mandatory entry that MUST be included (and the first)
	in all of the entries.
*/
#define GDT_MAX_ENTRIES 3

#define GDT_DESCRIPTOR_NULL 0
#define GDT_DESCRIPTOR_CODE 1
#define GDT_DESCRIPTOR_DATA 2

#define GDT_ADDRESS_MIN 0
#define GDT_ADDRESS_MAX 0xFFFFF

// Invoked from assembly
extern void gdt_flush(uint32_t);

static void gdt_set_gate(int32_t idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);

static struct gdt_entry entries[GDT_MAX_ENTRIES];

static struct gdt_ptr ptr;



void gdt_init() {
	ptr.limit = (sizeof(struct gdt_entry) * GDT_MAX_ENTRIES) - 1;
	ptr.base = (uint32_t) &entries;

	/*
		The below sets up each descriptor accordingly. They maintain the following format...

		gdt_set_gate(
			[DESCRIPTOR], [ADDR_MIN], [ADDR_MAX],
			[ACCESS_BITS],
			[FLAGS_BITS]
		);

		Where DESCRIPTOR is mapped directly to it's corresponding index.
	*/
	gdt_set_gate(
		GDT_DESCRIPTOR_NULL, GDT_ADDRESS_MIN, GDT_ADDRESS_MIN,
		GDT_ACCESS_NONE,
		GDT_FLAGS_NONE
	);
	gdt_set_gate(
		GDT_DESCRIPTOR_CODE, GDT_ADDRESS_MIN, GDT_ADDRESS_MAX,
		GDT_ACCESS_RW | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_PRESENT,
		GDT_FLAGS_SIZE | GDT_FLAGS_GRANULARITY
	);
	gdt_set_gate(
		GDT_DESCRIPTOR_DATA, GDT_ADDRESS_MIN, GDT_ADDRESS_MAX,
		GDT_ACCESS_RW | GDT_ACCESS_PRESENT,
		GDT_FLAGS_SIZE | GDT_FLAGS_GRANULARITY
	);

	// Update the CPU's GDT
	gdt_flush((uint32_t) &ptr);
}

static void gdt_set_gate(int32_t idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
	// The Access and Flags bytes require certain bits to be set or unset.
	access |= GDT_ACCESS_GARBAGE;
	flags &= ~(GDT_FLAGS_GARBAGE);

	/*
		The base is encoded in a way such that the first 16 bits (0xFFFF) is encoded in base_low.
		The next 8 bits (0xFF) AFTER the first 16 bits (base >> 16) is encoded into base_mid.
		The last 8 bits (0xFF) AFTER the first 24 bits (base >> 24) is encoded into base_high.
	*/
	entries[idx].base_low = (base & 0xFFFF);
	entries[idx].base_mid = (base >> 16) & 0xFF;
	entries[idx].base_high = (base >> 24) & 0xFF;

	/*
		The limit is also encoded in such a way that the first 16 bits (0xFFFF) is stored in limit_low.
		The next 4 bits (0x0F) AFTER the first 16 bits (limit >> 16) is encoded into limit_high.
	*/
	entries[idx].limit_low = (limit & 0xFFFF);
	entries[idx].limit_high = (limit >> 16) & 0x0F;

	/*
		Flags and Access bytes can be set directly, as any width differences are truncated.
	*/
	entries[idx].flags = flags;
	entries[idx].access = access;
}