#include <include/kbd.h>
#include <include/vga.h>
#include <stdbool.h>
#include <include/idt.h>
#include <include/io_port.h>

static uint8_t (*scan_to_char(uint8_t));

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

static uint8_t state = 0;

static void keyboard_irq_handler(struct registers *regs) {
	// Get what key has been pressed
	uint8_t scancode = inb(KBD_PORT);

	// Multibyte Sequence, change state
	if (scancode == 0xE0) {
		state = 1;
		return;
	}

	bool released = scancode & 0x80;
	scancode &= ~0x80;

	vga_clear();
	vga_set_x(0);
	printf("Scancode: %x, You %s: ", scancode, released ? "Released" : "Pressed");

	switch (state) {
		// Waiting for first byte
		case 0:
			printf("%s", to_string(KBD_SCAN_TABLE[scancode]));
			return;
		// Wait
			ing for second byte of multibyte sequence
		case 1:
			printf("%s", escaped_to_string(KBD_ESCAPED_SCAN_TABLE[scancode]));
			state = 0;
	}
}

void keyboard_init() {
	register_interrupt_handler(IRQ1, keyboard_irq_handler);
}
