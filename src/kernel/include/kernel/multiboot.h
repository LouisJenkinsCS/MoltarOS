#ifndef MOLTAROS_MULTIBOOT_H
#define MOLTAROS_MULTIBOOT_H

#include <include/kernel/logger.h>
#include <stdint.h>

#define MULTIBOOT_MMAP_RAM 1

// GRUB's Multiboot information structure, which we push on the stack for use during kernel_init.
// It is used to determine hardware information, such as the amount of available RAM.
struct multiboot_info {
	// Determines what information is available in this structure
	uint32_t flags;

	// Only valid if bit 0 of flags is set.
	// Indicate amount of low and high memory respectively
	uint32_t mem_low;
	uint32_t mem_high;

	// Only valid if bit 1 of flags is set.
	// Indicates which disk device we were loaded from.
	uint32_t boot_device;

	// Only valid if bit 2 of flags is set.
	uint32_t cmdline;

	// Only valid if bit 3 of flags is set.
	// Indicates the number of boot modules loaded
	// alongside us, with the addr being the physical
	// address an array of them.
	uint32_t mod_count;
	uint32_t mod_addr;

	// Symbol table, which we ignore
	uint8_t sym_table[16];

	// Only valid if bit 6 of flags is set.
	// Describe the memory mappings and the size
	// of their buffers. Of notable importance is
	// RAM, which we use to obtain physical memory size. 
	uint32_t mmap_length;
	uint32_t mmap_addr;
};

// GRUB's memory map structure. The 'size' field is located at offset -4
// which means we have to take special care when going to the next entry.
struct multiboot_mmap {
	// Size of this entry.
	uint32_t size;

	// The start address
	uint32_t start_low;
	uint32_t start_high;

	// The length of this memory mapping
	uint32_t length_low;
	uint32_t length_high;

	// Whether this memory mapping is for RAM or some reserved area.
	uint32_t type;
};


static bool multiboot_RAM(struct multiboot_info *mbinfo, uint32_t *start, uint32_t *end) {
	bool found = false;
	// Check if there is a memory mapping available
	if (mbinfo->flags & (1 << 6)) {
		// KLOG("MMAP Entries: %d", mbinfo->mmap_length / 24);
		struct multiboot_mmap *mmap = (struct multiboot_mmap *) (0xC0000000 + mbinfo->mmap_addr);
		while((uint32_t) mmap - 0xC0000000 < (mbinfo->mmap_addr + mbinfo->mmap_length)) {
			// KLOG("MMAP Entry: {Type: %s, Start: %x, Length: %x}", mmap->type == MULTIBOOT_MMAP_RAM ? "RAM" : "RESERVED", mmap->start_low, mmap->length_low);
			// Jackpot... (The OS is 32-bit, so there is no upper currently.)
			if (mmap->type == MULTIBOOT_MMAP_RAM) {
				*start = mmap->start_low;
				*end = *start + mmap->length_low;
				found = true;
			}

			mmap = (struct multiboot_mmap *) ((uint32_t) mmap + mmap->size + sizeof(mmap->size));
		}
	}
	
	return found;
}

#endif /* MOLTAROS_MULTIBOOT_H */