#ifndef MOLTAROS_IDT_H
#define MOLTAROS_IDT_H

#include <stdint.h>

#define IDT_FLAGS_GATE_TASK 0x5
#define IDT_FLAGS_GATE_INTERRUPT16 0x6
#define IDT_FLAGS_GATE_INTERRUPT32 0xE
#define IDT_FLAGS_GATE_TRAP16 0x7
#define IDT_FLAGS_GATE_TRAP32 0xF
#define IDT_FLAGS_STORAGE_SEGMENT 1 << 4
#define IDT_FLAGS_PRIVILEDGE_RING_ZERO 0 << 5
#define IDT_FLAGS_PRIVILEDGE_RING_ONE 1 << 5
#define IDT_FLAGS_PRIVILEDGE_RING_TWO 2 << 5
#define IDT_FLAGS_PRIVILEDGE_RING_THREE 3 << 5
#define IDT_FLAGS_PRESENT 1 << 7

/*
	An entry in the interrupt descriptor table, which maintains the address (offset) to the interrupt
	handler, 
*/
struct __attribute__((packed)) idt_entry {
	// Lower 16-bits of the interrupt handler address
	uint16_t addr_low;
	// Index of a kernel segment descriptor (I.E DS or CS)
	uint16_t selector;
	// Should always be zero
	uint8_t _garbage;
	// Various flags, see IDT_FLAGS_* above
	uint8_t flags;
	// Last 16-bits of the interrupt handler address
	uint16_t addr_high;
};

/*
	The structure needed by the x86 instruction lidt to assign the CPU a new interrupt descriptor table
*/
struct __attribute__((packed)) idt_ptr {
	// Size of all interrupt entries minus one.
	uint16_t limit;
	// The address of the first entry in an array of idt_entry
	uint32_t base;
};

/*
	Structure of which the CPU will align and push each register on the stack. The stack pointer (esp)
	is converted into this structure to allow easier access to the registers directly.
*/
struct registers {
	// Index of the current data segment
	uint32_t ds;

	// Registers which were pushed during pusha instruction
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;

	// Interrupt number
	uint32_t int_num;
	// Error code
	uint32_t err_code;

	// Pushed automatically by the CPU, contain registers and information
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t useresp;
	uint32_t ss;
};

typedef void (*interrupt_handler)(struct registers *);

void idt_init();

/*
	Registers an interrupt handler to the specified interrupt number
*/
void register_interrupt_handler(uint8_t int_num, void (*handler)(struct registers *));

/*
	Below are generic interrupt handler functions that can be used to call assembly labels.

	TODO: Please make this less generic!
*/
extern void interrupt_service_request_0();
extern void interrupt_service_request_1();
extern void interrupt_service_request_2();
extern void interrupt_service_request_3();
extern void interrupt_service_request_4();
extern void interrupt_service_request_5();
extern void interrupt_service_request_6();
extern void interrupt_service_request_7();
extern void interrupt_service_request_8();
extern void interrupt_service_request_9();
extern void interrupt_service_request_10();
extern void interrupt_service_request_11();
extern void interrupt_service_request_12();
extern void interrupt_service_request_13();
extern void interrupt_service_request_14();
extern void interrupt_service_request_15();
extern void interrupt_service_request_16();
extern void interrupt_service_request_17();
extern void interrupt_service_request_18();
extern void interrupt_service_request_19();
extern void interrupt_service_request_20();
extern void interrupt_service_request_21();
extern void interrupt_service_request_22();
extern void interrupt_service_request_23();
extern void interrupt_service_request_24();
extern void interrupt_service_request_25();
extern void interrupt_service_request_26();
extern void interrupt_service_request_27();
extern void interrupt_service_request_28();
extern void interrupt_service_request_29();
extern void interrupt_service_request_30();
extern void interrupt_service_request_31();
extern void interrupt_service_request_255();

extern void interrupt_request_0 ();
extern void interrupt_request_1 ();
extern void interrupt_request_2 ();
extern void interrupt_request_3 ();
extern void interrupt_request_4 ();
extern void interrupt_request_5 ();
extern void interrupt_request_6 ();
extern void interrupt_request_7 ();
extern void interrupt_request_8 ();
extern void interrupt_request_9 ();
extern void interrupt_request_10();
extern void interrupt_request_11();
extern void interrupt_request_12();
extern void interrupt_request_13();
extern void interrupt_request_14();
extern void interrupt_request_15();

#endif /* MOLTAROS_IDT_H */