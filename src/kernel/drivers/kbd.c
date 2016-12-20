#include <include/kbd.h>
#include <include/vga.h>
#include <stdbool.h>
#include <include/idt.h>
#include <include/io_port.h>

bool shift_held = false;

static void keyboard_irq_handler(struct registers *regs) {
	// Get what key has been pressed
	uint8_t scancode = inb(KBD_PORT);

	// Check if top bit is set to see if key was released
	if (scancode & 0x80) {
		scancode &= ~0x80;
		if (scancode == LSHIFT) {
			shift_held = false;
		} else {
			// Button released...
			return;
		}
	}

	struct scan_code sc = SCAN_CODES[scancode];
	uint8_t ch = shift_held ? sc.escaped : sc.normal;
	switch (ch) {
		case 0x0: {
			break;
		}
		case BACKSPACE: {
			size_t x = vga_get_x();
			if (x != 0) {
				vga_set_x(x - 1);
				vga_putc(' ');
				vga_set_x(x - 1);
			}
			break;
		}
		case LSHIFT: {
			shift_held = true;
			break;
		}
		default: {
			vga_putc(ch);
		}
	}
}

void keyboard_init() {
	register_interrupt_handler(IRQ1, keyboard_irq_handler);
}
