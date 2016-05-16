#ifndef MOLTAROS_GDT_H
#define MOLTAROS_GDT_H

static struct __attribute__((packed)) {
	unsigned lower_limit : 16;
	unsigned lower_base : 16;
	unsigned mid_base : 8;
 
	// Accept byte
	unsigned access_accessed : 1;
	unsigned access_rw : 1;
	unsigned access_direction : 1;
	unsigned access_execution : 1;
	unsigned access_garbage : 1;
	unsigned access_priviledge : 2;
	unsigned access_present : 1;
 
	unsigned upper_limit : 4;
 
	// Flags byte
	unsigned flags_garbage : 2;
	unsigned flags_size : 1;
	unsigned flags_granularity : 1;
 
	unsigned upper_base : 8;
} gdt;

#endif /* MOLTAROS_GDT_H */