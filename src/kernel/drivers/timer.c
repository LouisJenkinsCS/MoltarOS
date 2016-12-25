#include <include/drivers/timer.h>
#include <include/x86/io_port.h>
#include <include/helpers.h>
#include <stdio.h>
#include <limits.h>

static const uint32_t oscillator_frequency = 1193180;

static void timer_default_irq(struct registers *UNUSED(regs)) {
	// NOP
}

void timer_init() {
	register_interrupt_handler(IRQ0, timer_default_irq);
}

void timer_set_handler(uint32_t frequency, void (*cb)(struct registers *registers)) {
	// Note: Frequency MUST be large enough so that rate fits within a 2-byte range
	uint16_t rate = (uint16_t) (oscillator_frequency / frequency);

	// Whenever we receive an interrupt from the system timer...
	register_interrupt_handler(IRQ0, cb);

	// Set PIT mode to repeatedly interrupt at the requested frequency (below)
	outb(PIT_COMMAND, PIT_REPEAT);

	// Split the 16-bit rate into two 8-bit numbers
	uint8_t low = (uint8_t)(rate & 0xFF);
	uint8_t high = (uint8_t)((rate >> 8) & 0xFF);

	// Send new frequency on channel 0
	outb(PIT_CHANNEL0, low);
	outb(PIT_CHANNEL0, high);
}