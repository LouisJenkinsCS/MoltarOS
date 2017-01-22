#ifndef MOLTAROS_PROCESS_H
#define MOLTAROS_PROCESS_H

#include <include/x86/idt.h>
#include <stddef.h>

typedef void (*task_fp)(void *);

typedef struct process {
	regs_t regs;
	size_t id;
	struct process *next;
} proc_t;

void proc_init();

int fork();

#endif /* endif MOLTAROS_PROCESS_H */