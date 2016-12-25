#include <include/drivers/kbd.h>
#include <include/drivers/vga.h>
#include <include/x86/idt.h>
#include <include/x86/io_port.h>
#include <include/helpers.h>
#include <stdbool.h>
#include <stdio.h>

// Below are the two scan tables for normal scan codes, used to provide a mapping from scan code to character code. 
// A scan table entry of [0x01] = KBD_KEY_ESC would have a scan code of 0x01, and a character code of KBD_KEY_ESC.
static const uint8_t KBD_SCAN_TABLE[KBD_NSCANS] = {
    [0x01] = KBD_KEY_ESC, 
    [0x02] = KBD_KEY_1, 
    [0x03] = KBD_KEY_2,
    [0x04] = KBD_KEY_3,
    [0x05] = KBD_KEY_4,
    [0x06] = KBD_KEY_5,
    [0x07] = KBD_KEY_6,
    [0x08] = KBD_KEY_7,
    [0x09] = KBD_KEY_8,
    [0x0A] = KBD_KEY_9,
    [0x0B] = KBD_KEY_0,
    [0x0C] = KBD_KEY_MINUS,
    [0x0D] = KBD_KEY_EQUALS,
    [0x0E] = KBD_KEY_BACKSPACE,
    [0x0F] = KBD_KEY_TAB,
    [0x10] = KBD_KEY_Q,
    [0x11] = KBD_KEY_W,
    [0x12] = KBD_KEY_E,
    [0x13] = KBD_KEY_R,
    [0x14] = KBD_KEY_T,
    [0x15] = KBD_KEY_Y,
    [0x16] = KBD_KEY_U,
    [0x17] = KBD_KEY_I,
    [0x18] = KBD_KEY_O,
    [0x19] = KBD_KEY_P,
    [0x1A] = KBD_KEY_LBRACKET,
    [0x1B] = KBD_KEY_RBRACKET,
    [0x1C] = KBD_KEY_ENTER,
    [0x1D] = KBD_KEY_LCTRL,
    [0x1E] = KBD_KEY_A,
    [0x1F] = KBD_KEY_S,
    [0x20] = KBD_KEY_D,
    [0x21] = KBD_KEY_F,
    [0x22] = KBD_KEY_G,
    [0x23] = KBD_KEY_H,
    [0x24] = KBD_KEY_J,
    [0x25] = KBD_KEY_K,
    [0x26] = KBD_KEY_L,
    [0x27] = KBD_KEY_SEMICOLON,
    [0x28] = KBD_KEY_APOSTROPH,
    [0x29] = KBD_KEY_GRAVE_ACCENT,
    [0x2A] = KBD_KEY_LSHIFT,
    [0x2B] = KBD_KEY_BACK_SLASH,
    [0x2C] = KBD_KEY_Z,
    [0x2D] = KBD_KEY_X,
    [0x2E] = KBD_KEY_C,
    [0x2F] = KBD_KEY_V,
    [0x30] = KBD_KEY_B,
    [0x31] = KBD_KEY_N,
    [0x32] = KBD_KEY_M,
    [0x33] = KBD_KEY_COMMA,
    [0x34] = KBD_KEY_PERIOD,
    [0x35] = KBD_KEY_SLASH,
    [0x36] = KBD_KEY_RSHIFT,
    [0x37] = KBD_KP_KEY_ASTERISK,
    [0x38] = KBD_KEY_LALT,
    [0x39] = KBD_KEY_SPACE,
    [0x3A] = KBD_KEY_CAPS_LOCK,
    [0x3B] = KBD_KEY_F1,
    [0x3C] = KBD_KEY_F2,
    [0x3D] = KBD_KEY_F3,
    [0x3E] = KBD_KEY_F4,
    [0x3F] = KBD_KEY_F5,
    [0x40] = KBD_KEY_F6,
    [0x41] = KBD_KEY_F7,
    [0x42] = KBD_KEY_F8,
    [0x43] = KBD_KEY_F9,
    [0x44] = KBD_KEY_F10,
    [0x45] = KBD_KEY_NUMLOCK,
    [0x46] = KBD_KEY_SCROLL_LOCK,
    [0x47] = KBD_KP_KEY_7,
    [0x48] = KBD_KP_KEY_8,
    [0x49] = KBD_KP_KEY_9,
    [0x4A] = KBD_KP_KEY_MINUS,
    [0x4B] = KBD_KP_KEY_4,
    [0x4C] = KBD_KP_KEY_5,
    [0x4D] = KBD_KP_KEY_6,
    [0x4E] = KBD_KP_KEY_PLUS,
    [0x4F] = KBD_KP_KEY_1,
    [0x50] = KBD_KP_KEY_2,
    [0x51] = KBD_KP_KEY_3,
    [0x52] = KBD_KP_KEY_0,
    [0x53] = KBD_KP_KEY_PERIOD,
    [0x57] = KBD_KEY_F11,
    [0x58] = KBD_KEY_F12
};

// Escaped scan code sequences are multibyte, and begin with '0xE0'.
static const uint8_t KBD_ESCAPED_SCAN_TABLE[KBD_NSCANS] = {
    [0x1C] = KBD_KP_KEY_ENTER,
    [0x1D] = KBD_KEY_RCTRL,
    [0x35] = KBD_KP_KEY_SLASH,
    [0x38] = KBD_KEY_RALT,
    [0x48] = KBD_KEY_UP,
    [0x49] = KBD_KEY_PAGE_UP,
    [0x4B] = KBD_KEY_LEFT,
    [0x4D] = KBD_KEY_RIGHT,
    [0x4F] = KBD_KEY_PAGE_DOWN,
    [0x50] = KBD_KEY_DOWN,
    [0x51] = KBD_KEY_PAGE_DOWN,
    [0x52] = KBD_KEY_INSERT,
    [0x53] = KBD_KEY_DELETE
};


// Converts a character code to string
static const char *to_string(uint8_t code) {
	const char *str;
	switch (code) {
		case KBD_KEY_ESC:
			str = "ESC";
			break; 
		case KBD_KEY_F1:
			str = "F1";
			break; 
		case KBD_KEY_F2:
			str = "F2";
			break; 
		case KBD_KEY_F3:
			str = "F3";
			break; 
		case KBD_KEY_F4:
			str = "F4";
			break; 
		case KBD_KEY_F5:
			str = "F5";
			break; 
		case KBD_KEY_F6:
			str = "F6";
			break;
		case KBD_KEY_F7:
			str = "F7";
			break; 
		case KBD_KEY_F8:
			str = "F8";
			break; 
		case KBD_KEY_F9:
			str = "F9";
			break; 
		case KBD_KEY_F10:
			str = "F10";
			break; 
		case KBD_KEY_F11:
			str = "F11";
			break; 
		case KBD_KEY_F12:
			str = "F12";
			break; 
		case KBD_KEY_PRTSCR:
			str = "PRTSCR";
			break;
		case KBD_KEY_SCROLL_LOCK:
			str = "SCROLL_LOCK";
			break; 
		case KBD_KEY_PAUSE:
			str = "PAUSE";
			break;   
		case KBD_KEY_GRAVE_ACCENT:
			str = "GRAVE_ACCENT";
			break; 
		case KBD_KEY_1:
			str = "1";
			break; 
		case KBD_KEY_2:
			str = "2";
			break; 
		case KBD_KEY_3:
			str = "3";
			break; 
		case KBD_KEY_4:
			str = "4";
			break; 
		case KBD_KEY_5:
			str = "5";
			break; 
		case KBD_KEY_6:
			str = "6";
			break; 
		case KBD_KEY_7:
			str = "7";
			break; 
		case KBD_KEY_8:
			str = "8";
			break; 
		case KBD_KEY_9:
			str = "9";
			break;
		case KBD_KEY_0:
			str = "0";
			break; 
		case KBD_KEY_MINUS:
			str = "MINUS";
			break; 
		case KBD_KEY_EQUALS:
			str = "EQUALS";
			break; 
		case KBD_KEY_BACKSPACE:
			str = "BACKSPACE";
			break; 
		case KBD_KEY_NUMLOCK:
			str = "NUMLOCK";
			break; 
		case KBD_KP_KEY_ASTERISK:
			str = "(keypad) ASTERISK";
			break;
		case KBD_KP_KEY_MINUS:
			str = "(keypad) MINUS";
			break; 
		case KBD_KEY_TAB:
			str = "TAB";
			break; 
		case KBD_KEY_Q:
			str = "Q";
			break; 
		case KBD_KEY_W:
			str = "W";
			break; 
		case KBD_KEY_E:
			str = "E";
			break; 
		case KBD_KEY_R:
			str = "R";
			break; 
		case KBD_KEY_T:
			str = "T";
			break; 
		case KBD_KEY_Y:
			str = "Y";
			break; 
		case KBD_KEY_U:
			str = "U";
			break; 
		case KBD_KEY_I:
			str = "I";
			break; 
		case KBD_KEY_O:
			str = "O";
			break;
		case KBD_KEY_P:
			str = "P";
			break; 
		case KBD_KEY_LBRACKET:
			str = "LBRACKET";
			break; 
		case KBD_KEY_RBRACKET:
			str = "RBRACKET";
			break; 
		case KBD_KEY_BACK_SLASH:
			str = "BACK_SLASH";
			break; 
		case KBD_KP_KEY_7:
			str = "(keypad) 7";
			break; 
		case KBD_KP_KEY_8:
			str = "(keypad) 8";
			break; 
		case KBD_KP_KEY_9:
			str = "(keypad) 9";
			break; 
		case KBD_KP_KEY_PLUS:
			str = "(keypad) PLUS";
			break;
		case KBD_KEY_CAPS_LOCK:
			str = "CAPS_LOCK";
			break; 
		case KBD_KEY_A:
			str = "A";
			break; 
		case KBD_KEY_S:
			str = "S";
			break; 
		case KBD_KEY_D:
			str = "D";
			break; 
		case KBD_KEY_F:
			str = "F";
			break; 
		case KBD_KEY_G:
			str = "G";
			break; 
		case KBD_KEY_H:
			str = "H";
			break; 
		case KBD_KEY_J:
			str = "J";
			break; 
		case KBD_KEY_K:
			str = "K";
			break; 
		case KBD_KEY_L:
			str = "L";
			break; 
		case KBD_KEY_SEMICOLON:
			str = "SEMICOLON";
			break;
		case KBD_KEY_APOSTROPH:
			str = "APOSTROPH";
			break; 
		case KBD_KEY_ENTER:
			str = "ENTER";
			break; 
		case KBD_KP_KEY_4:
			str = "(keypad) 4";
			break; 
		case KBD_KP_KEY_5:
			str = "(keypad) 5";
			break; 
		case KBD_KP_KEY_6:
			str = "(keypad) 6";
			break; 
		case KBD_KEY_LSHIFT:
			str = "LSHIFT";
			break; 
		case KBD_KEY_Z:
			str = "Z";
			break; 
		case KBD_KEY_X:
			str = "X";
			break; 
		case KBD_KEY_C:
			str = "C";
			break;
		case KBD_KEY_V:
			str = "V";
			break; 
		case KBD_KEY_B:
			str = "B";
			break; 
		case KBD_KEY_N:
			str = "N";
			break; 
		case KBD_KEY_M:
			str = "M";
			break; 
		case KBD_KEY_COMMA:
			str = "COMMA";
			break; 
		case KBD_KEY_PERIOD:
			str = "PERIOD";
			break; 
		case KBD_KEY_SLASH:
			str = "SLASH";
			break; 
		case KBD_KEY_RSHIFT:
			str = "RSHIFT";
			break; 
		case KBD_KP_KEY_1:
			str = "(keypad) 1";
			break;
		case KBD_KP_KEY_2:
			str = "(keypad) 2";
			break; 
		case KBD_KP_KEY_3:
			str = "(keypad) 3";
			break; 
		case KBD_KEY_LCTRL:
			str = "LCTRL";
			break;
		case KBD_KEY_FN:
			str = "FN";
			break;
		case KBD_KEY_LALT:
			str = "LALT";
			break;
		case KBD_KEY_SPACE:
			str = "SPACE";
			break; 
		case KBD_KEY_BACK_SLASH2:
			str = "BACK_SLASH2";
			break;
		case KBD_KEY_SUPER:
			str = "SUPER";
			break;   
		case KBD_KP_KEY_0:
			str = "(keypad) 0";
			break; 
		case KBD_KP_KEY_PERIOD:
		    str = "(keypad) PERIOD";
			break;
	    default:
	    	str = "(NULL)";
			break;
	}

	return str;
}

// Converts escaped character codes to string
static const char *escaped_to_string(uint8_t code) {
	const char *str;
	switch (code) {
		case KBD_KP_KEY_ENTER:
			str = "(keypad) ENTER";
			break;
		case KBD_KEY_RCTRL:
			str = "RCTRL";
			break; 
		case KBD_KP_KEY_SLASH:
			str = "(keypad) SLASH";
			break;
		case KBD_KEY_RALT:
			str = "RALT";
			break;
		case KBD_KEY_UP:
			str = "UP";
			break;
		case KBD_KEY_PAGE_UP:
			str = "PAGE_UP";
			break;
		case KBD_KEY_LEFT:
			str = "LEFT";
			break;
		case KBD_KEY_RIGHT:
			str = "RIGHT";
			break;
		case KBD_KEY_PAGE_DOWN:
			str = "PAGE_DOWN";
			break;
		case KBD_KEY_DOWN:
			str = "DOWN";
			break;
		case KBD_KEY_DELETE:
			str = "DELETE";
			break;
		case KBD_KEY_INSERT:
			str = "INSERT";
			break;
	}

	return str;
}

// Keyboard state, which drives the statement machine in handler
static uint8_t state = 0;

// A simple (and temporary) handler that reads input from the hardware (keyboard)
// and converts it into it's respective character codes.
// Temporary: It currently prints out the key being pressed but that is because
// I hav enot implemented a more adequate way of doing this.
static void keyboard_irq_handler(struct registers *UNUSED(regs)) {
	// Get what key has been pressed
	uint8_t scancode = inb(KBD_PORT);

	// Multibyte Sequence, change state
	if (scancode == 0xE0) {
		state = 1;
		return;
	}

	// All scan codes signify that a key has been released by setting the most significant bit
	bool released = scancode & 0x80;
	scancode &= ~0x80;

	// On each key press, we write prettily to the same line.
	vga_clear();
	vga_set_x(0);
	printf("Scancode: %x, You %s: ", scancode, released ? "Released" : "Pressed");

	// State-Based machine, which determines which scan table to decode from.
	switch (state) {
		// Waiting for first byte
		case 0:
			printf("%s", to_string(KBD_SCAN_TABLE[scancode]));
			return;
		// Waiting for second byte of multibyte sequence
		case 1:
			printf("%s", escaped_to_string(KBD_ESCAPED_SCAN_TABLE[scancode]));
			state = 0;
	}
}

// Simple initializer that registers IRQ handler
void keyboard_init() {
	register_interrupt_handler(IRQ1, keyboard_irq_handler);
}
