#ifndef MOLTAROS_KEYBOARD_H
#define MOLTAROS_KEYBOARD_H

#define KBD_PORT 0x60
#define RELEASE_MASK 0x80

#import <stdint.h>

struct kbd_event {
    // Held Key States
    bool shift : 1;
    bool alt : 1;
    bool ctrl : 1;
    bool fn : 1;

    // Toggle Key States
    bool caps : 1;
    bool scroll : 1;
    bool num : 1;

    // Pressed/Released State
    bool state : 1;
    // Note: All the above is packed into 1 byte. :)

    // Mapped Key Code
    uint8_t keycode;
};

struct scan_code {
  uint8_t normal;
  uint8_t escaped;
};

enum {
    KBD_ESC = 0x1, KBD_F1, KBD_F2, KBD_F3, KBD_F4, KBD_F5, KBD_F6,
    KBD_F7, KBD_F8, KBD_F9, KBD_F10, KBD_F11, KBD_F12, KBD_PRTSCR,
    KBD_SCROLL_LOCK, KBD_PAUSE, KBD_INSERT, KBD_DELETE, KBD_PAGE_UP, KBD_PAGE_DOWN,
    KBD_GRAVE_ACCENT, KBD_1, KBD_2, KBD_3, KBD_4, KBD_5, KBD_6, KBD_7, KBD_8, KBD_9,
    KBD_0, KBD_MINUS, KBD_EQUALS, KBD_BACKSPACE, KBD_NUMLOCK, KBD_KP_SLASH, KBD_KP_ASTERISK,
    KBD_KP_MINUS, KBD_TAB, KBD_Q, KBD_W, KBD_E, KBD_R, KBD_T, KBD_Y, KBD_U, KBD_I, KBD_O,
    KBD_P, KBD_LBRACKET, KBD_RBRACKET, KBD_BACK_SLASH, KBD_KP_7, KBD_KP_8, KBD_KP_9, KBD_KP_PLUS
};

#define ESC 0x1
#define BACKSPACE 0x0E
#define TAB 0x0F
#define ENTER 0x1C
#define LCTRL 0x1D
#define LSHIFT 0x2A
#define RSHIFT 0x36
#define LALT 0x38
#define CAPS_LOCK 0x3a
#define NUMS_LOCK 0x45
#define SCROLL_LOCK 0x46

// Function buttons
enum {
    F1 = 0x3b, F2, F3, F4, F5, F6, F7, F8, F9, F10,
    F11 = 0x57, F12
};

static const struct scan_code SCAN_CODES[0x80] = {
    // Error Code
    {0x0, 0x0},
    
    // Keyboard Keys
    {ESC, ESC}, {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, 
    {'5', '%'}, {'6', '^'}, {'7', '&'}, {'8', '*'}, {'9', '('}, 
    {'0', ')'}, {'-', '_'}, {'=', '+'}, {BACKSPACE, BACKSPACE}, {TAB, TAB}, 
    {'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'}, {'t', 'T'}, 
    {'y', 'Y'}, {'u', 'U'}, {'i', 'I'}, {'o', 'O'}, {'p', 'P'}, 
    {'[', '{'}, {']', '}'}, {ENTER, ENTER}, {LCTRL, LCTRL}, {'a', 'A'}, 
    {'s', 'S'}, {'d', 'D'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'}, 
    {'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {';', ':'}, {'\'', '\"'}, 
    {'`', '~'}, {LSHIFT, LSHIFT}, {'\\', '|'}, {'z', 'Z'}, {'x', 'X'}, 
    {'c', 'C'}, {'v', 'V'}, {'b', 'B'}, {'n', 'N'}, {'m', 'M'}, 
    {',', '<'}, {'.', '>'}, {'/', '?'}, {RSHIFT, RSHIFT}, {0x0, 0x0}, 
    {LALT, LALT}, {' ', ' '}, {CAPS_LOCK, CAPS_LOCK}, {F1, F1}, {F2, F2}, 
    {F3, F3}, {F4, F4}, {F5, F5}, {F6, F6}, {F7, F7}, 
    {F8, F8}, {F9, F9}, {F10, F10}, {NUMS_LOCK, NUMS_LOCK}, 
    {SCROLL_LOCK, SCROLL_LOCK}, {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, 
    {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, 
    {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, {0x0, 0x0}, 
    {0x0, 0x0}, {F11, F11}, {F12, F12}
};

void keyboard_init();

#endif /* endif MOLTAROS_KEYBOARD_H */