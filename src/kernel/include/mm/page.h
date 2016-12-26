#ifndef MOLTAROS_PAGE_H
#define MOLTAROS_PAGE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct page_entry {
	// Is this entry present in memory
	uint32_t present : 1;
	// Is this entry read-only or read-write
	uint32_t read_write : 1;
	// Is this entry in supervisor-mode or user-mode 
	uint32_t user_mode : 1;
	// Has this entry been accessed before? (Set by CPU)
	uint32_t accessed : 1;
	// Has this entry been written to? (Set by CPU)
	bool dirty : 1;
	// Reserved
	uint32_t  __unused__ : 7;
	// Higher 20 bits of frame address (frame_addr << 12)
	uint32_t frame_addr : 20;
} __attribute__((packed)) page_entry_t;

typedef struct page_table {
	// A 4KB chunk of page entries
	page_entry_t pages[1024];
} page_table_t;

typedef struct page_directory {
	// A 4KB chunk of pointers to page tables. Unlike
	// page_table_t's pages, which take up a constant amount
	// of space, unused tables can be deallocated to save space.
	page_table_t *tables[1024];
	// The mapping of physical addresses for each table, managed
	// because the CR3 register requires this.
	// tables_addr[i] is the physical address of tables[i].
	uint32_t physical_tables[1024];
	// The physical address of tables_addr for cases where this
	// directory is allocated in virtual memory, which arises when
	// cloning page directories.
	uint32_t physical_addr;
} page_directory_t;

void page_init();

void page_alloc(page_entry_t *entry, bool kernel, bool write);

void page_free(page_entry_t *entry);

#endif