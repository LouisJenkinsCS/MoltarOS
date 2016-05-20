#include <include/kernel/idt.h>
#include <include/kernel/io_port.h>
#include <string.h>
#include <stdio.h>

#define CODE_DESCR 0x8

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_MASTER_START_OFFSET 0x20
#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1
#define PIC_SLAVE_START_OFFSET 0x28
#define PIC_EOI 0x20

#define IDT_MAX_ENTRIES 256

static struct idt_entry entries[IDT_MAX_ENTRIES];

static struct idt_ptr ptr;

static interrupt_handler handlers[IDT_MAX_ENTRIES];


extern void idt_flush(uint32_t idt_ptr);

static void idt_set_gate(uint8_t int_num, uint32_t addr, uint16_t selector, uint8_t flags);

static void init_pic();

static void init_isr();

static void init_irq();


void idt_init() {
	// Zero each interrupt handler and entry entry (to avoid any unnecessary surprises)
	memset(&handlers, 0, sizeof(interrupt_handler) * IDT_MAX_ENTRIES);
	memset(&entries, 0, sizeof(struct idt_entry) * IDT_MAX_ENTRIES);

	ptr.limit = (sizeof(struct idt_entry) * IDT_MAX_ENTRIES) - 1;
	ptr.base = (uint32_t) &entries;

	// Remap the master & slave PICs.
	init_pic();

	// Initialize and setup proper gates
	init_isr();
	init_irq();

	// Update IDT in CPU
	idt_flush((uint32_t) &ptr);
}

/*
	Registers an interrupt handler to the specified interrupt number
*/
void register_interrupt_handler(uint8_t int_num, void (*handler)(struct registers *)) {
	handlers[int_num] = handler;
}

// This gets called from our ASM interrupt handler stub.
void idt_handler(struct registers *registers) {
	// If the handler exists...
	if(handlers[registers->int_num])
		handlers[registers->int_num](registers);
	else
		printf("\nUnexpected Interrupt: %x\n", registers->int_num);
}

void irq_handler(struct registers *registers) {

	// If this interrupt is meant for the slave, send EOI
	if(registers->int_num >= PIC_SLAVE_START_OFFSET)
		outb(PIC_SLAVE_COMMAND, PIC_EOI);

	// Send EOI to master
	outb(PIC_MASTER_COMMAND, PIC_EOI);

	if(handlers[registers->int_num])
		handlers[registers->int_num](registers);
	else
		printf("\nUnexpected Interrupt: %x\n", registers->int_num);
}



static void idt_set_gate(uint8_t int_num, uint32_t addr, uint16_t selector, uint8_t flags) {
	/*
		The first 16 bits (0xFFFF) are encoded in addr_low
		The last 16 bits (0xFFFF) after the first 16-bits (addr >> 16) are encoded into addr_high
	*/
	entries[int_num].addr_low = addr & 0xFFFF;
	entries[int_num].addr_high = (addr >> 16) & 0xFFFF;

	// The garbage byte must always be 0.
	entries[int_num]._garbage = 0;

	entries[int_num].selector = selector;
	entries[int_num].flags = flags;
}

static void init_pic() {
	// Initialize both PICs
	outb(PIC_MASTER_COMMAND, 0x11);
	outb(PIC_SLAVE_COMMAND, 0x11);

	// Master PIC begins at 32, Slave PIC begins at 40
	outb(PIC_MASTER_DATA, PIC_MASTER_START_OFFSET);
	outb(PIC_SLAVE_DATA, PIC_SLAVE_START_OFFSET);

	// Let Master and Slave PICs know they are communicating together.
	outb(PIC_MASTER_DATA, 0x04);
	outb(PIC_SLAVE_DATA, 0x02);

	// Lets PICs know that we should handle the interrupts
	outb(PIC_MASTER_DATA, 0x01);
	outb(PIC_SLAVE_DATA, 0x01);

	// Disable all IRQs
	outb(PIC_MASTER_DATA, 0x0);
	outb(PIC_SLAVE_DATA, 0x0);
}

static void init_isr() {
	uint8_t isr_flags = IDT_FLAGS_GATE_INTERRUPT32 | IDT_FLAGS_PRIVILEDGE_RING_ZERO | IDT_FLAGS_PRESENT;

	/*
		The format for this is as follows...

		idt_set_gate(
			[INTERRUPT_NUMBER], [ISR_ADDRESS], [SEGMENT_DESCRIPTOR],
			[FLAGS]
		);
	*/
	idt_set_gate(0, (uint32_t)interrupt_service_request_0, CODE_DESCR, isr_flags);
	idt_set_gate(1, (uint32_t)interrupt_service_request_1 , CODE_DESCR, isr_flags);
	idt_set_gate(2, (uint32_t)interrupt_service_request_2 , CODE_DESCR, isr_flags);
	idt_set_gate(3, (uint32_t)interrupt_service_request_3 , CODE_DESCR, isr_flags);
	idt_set_gate(4, (uint32_t)interrupt_service_request_4 , CODE_DESCR, isr_flags);
	idt_set_gate(5, (uint32_t)interrupt_service_request_5 , CODE_DESCR, isr_flags);
	idt_set_gate(6, (uint32_t)interrupt_service_request_6 , CODE_DESCR, isr_flags);
	idt_set_gate(7, (uint32_t)interrupt_service_request_7 , CODE_DESCR, isr_flags);
	idt_set_gate(8, (uint32_t)interrupt_service_request_8 , CODE_DESCR, isr_flags);
	idt_set_gate(9, (uint32_t)interrupt_service_request_9 , CODE_DESCR, isr_flags);
	idt_set_gate(10, (uint32_t)interrupt_service_request_10, CODE_DESCR, isr_flags);
	idt_set_gate(11, (uint32_t)interrupt_service_request_11, CODE_DESCR, isr_flags);
	idt_set_gate(12, (uint32_t)interrupt_service_request_12, CODE_DESCR, isr_flags);
	idt_set_gate(13, (uint32_t)interrupt_service_request_13, CODE_DESCR, isr_flags);
	idt_set_gate(14, (uint32_t)interrupt_service_request_14, CODE_DESCR, isr_flags);
	idt_set_gate(15, (uint32_t)interrupt_service_request_15, CODE_DESCR, isr_flags);
	idt_set_gate(16, (uint32_t)interrupt_service_request_16, CODE_DESCR, isr_flags);
	idt_set_gate(17, (uint32_t)interrupt_service_request_17, CODE_DESCR, isr_flags);
	idt_set_gate(18, (uint32_t)interrupt_service_request_18, CODE_DESCR, isr_flags);
	idt_set_gate(19, (uint32_t)interrupt_service_request_19, CODE_DESCR, isr_flags);
	idt_set_gate(20, (uint32_t)interrupt_service_request_20, CODE_DESCR, isr_flags);
	idt_set_gate(21, (uint32_t)interrupt_service_request_21, CODE_DESCR, isr_flags);
	idt_set_gate(22, (uint32_t)interrupt_service_request_22, CODE_DESCR, isr_flags);
	idt_set_gate(23, (uint32_t)interrupt_service_request_23, CODE_DESCR, isr_flags);
	idt_set_gate(24, (uint32_t)interrupt_service_request_24, CODE_DESCR, isr_flags);
	idt_set_gate(25, (uint32_t)interrupt_service_request_25, CODE_DESCR, isr_flags);
	idt_set_gate(26, (uint32_t)interrupt_service_request_26, CODE_DESCR, isr_flags);
	idt_set_gate(27, (uint32_t)interrupt_service_request_27, CODE_DESCR, isr_flags);
	idt_set_gate(28, (uint32_t)interrupt_service_request_28, CODE_DESCR, isr_flags);
	idt_set_gate(29, (uint32_t)interrupt_service_request_29, CODE_DESCR, isr_flags);
	idt_set_gate(30, (uint32_t)interrupt_service_request_30, CODE_DESCR, isr_flags);
	idt_set_gate(31, (uint32_t)interrupt_service_request_31, CODE_DESCR, isr_flags);
	idt_set_gate(255, (uint32_t)interrupt_service_request_255, CODE_DESCR, isr_flags);
}

static void init_irq() {
	uint8_t irq_flags = IDT_FLAGS_GATE_INTERRUPT32 | IDT_FLAGS_PRIVILEDGE_RING_ZERO | IDT_FLAGS_PRESENT;

	/*
		The format for this is as follows...

		idt_set_gate(
			[INTERRUPT_NUMBER], [IRQ_ADDRESS], [SEGMENT_DESCRIPTOR],
			[FLAGS]
		);
	*/
	idt_set_gate(32, (uint32_t)interrupt_request_0, CODE_DESCR, irq_flags);
	idt_set_gate(33, (uint32_t)interrupt_request_1, CODE_DESCR, irq_flags);
	idt_set_gate(34, (uint32_t)interrupt_request_2, CODE_DESCR, irq_flags);
	idt_set_gate(35, (uint32_t)interrupt_request_3, CODE_DESCR, irq_flags);
	idt_set_gate(36, (uint32_t)interrupt_request_4, CODE_DESCR, irq_flags);
	idt_set_gate(37, (uint32_t)interrupt_request_5, CODE_DESCR, irq_flags);
	idt_set_gate(38, (uint32_t)interrupt_request_6, CODE_DESCR, irq_flags);
	idt_set_gate(39, (uint32_t)interrupt_request_7, CODE_DESCR, irq_flags);
	idt_set_gate(40, (uint32_t)interrupt_request_8, CODE_DESCR, irq_flags);
	idt_set_gate(41, (uint32_t)interrupt_request_9, CODE_DESCR, irq_flags);
	idt_set_gate(42, (uint32_t)interrupt_request_10, CODE_DESCR, irq_flags);
	idt_set_gate(43, (uint32_t)interrupt_request_11, CODE_DESCR, irq_flags);
	idt_set_gate(44, (uint32_t)interrupt_request_12, CODE_DESCR, irq_flags);
	idt_set_gate(45, (uint32_t)interrupt_request_13, CODE_DESCR, irq_flags);
	idt_set_gate(46, (uint32_t)interrupt_request_14, CODE_DESCR, irq_flags);
	idt_set_gate(47, (uint32_t)interrupt_request_15, CODE_DESCR, irq_flags);
}