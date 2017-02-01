#ifndef MOLTAROS_TASK_H
#define MOLTAROS_TASK_H

#include <include/x86/idt.h>
#include <stddef.h>

typedef void (*task_fp)(void *);

typedef struct task {
	uint32_t eip;
	uint32_t esp;
	uint32_t ebp;
	uint32_t stack_start;
	size_t id;
} task_t;

void task_init();

void thread_create(void (*task)(void *args), void *args);

#endif /* endif MOLTAROS_TASK_H */