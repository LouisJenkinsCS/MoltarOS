#ifndef MOLTAROS_KEYBOARD_H
#define MOLTAROS_KEYBOARD_H

#define KBD_KEY_PORT 0x60
#define RELEASE_MASK 0x80

#import <stdint.h>

struct scan_code {
  uint8_t normal;
  uint8_t escaped;
};

enum {
    KBD_KEY_ESC = 0x1, 
    KBD_KEY_F1, 
    KBD_KEY_F2, 
    KBD_KEY_F3, 
    KBD_KEY_F4, 
    KBD_KEY_F5, 
    KBD_KEY_F6,
    KBD_KEY_F7, 
    KBD_KEY_F8, 
    KBD_KEY_F9, 
    KBD_KEY_F10, 
    KBD_KEY_F11, 
    KBD_KEY_F12, 
    KBD_KEY_PRTSCR,
    KBD_KEY_SCROLL_LOCK, 
    KBD_KEY_PAUSE, 
    KBD_KEY_INSERT, 
    KBD_KEY_DELETE, 
    KBD_KEY_PAGE_UP, 
    KBD_KEY_PAGE_DOWN,
    KBD_KEY_GRAVE_ACCENT, 
    KBD_KEY_1, 
    KBD_KEY_2, 
    KBD_KEY_3, 
    KBD_KEY_4, 
    KBD_KEY_5, 
    KBD_KEY_6, 
    KBD_KEY_7, 
    KBD_KEY_8, 
    KBD_KEY_9,
    KBD_KEY_0, 
    KBD_KEY_MINUS, 
    KBD_KEY_EQUALS, 
    KBD_KEY_BACKSPACE, 
    KBD_KEY_NUMLOCK, 
    KBD_KP_KEY_SLASH, 
    KBD_KP_KEY_ASTERISK,
    KBD_KP_KEY_MINUS, 
    KBD_KEY_TAB, 
    KBD_KEY_Q, 
    KBD_KEY_W, 
    KBD_KEY_E, 
    KBD_KEY_R, 
    KBD_KEY_T, 
    KBD_KEY_Y, 
    KBD_KEY_U, 
    KBD_KEY_I, 
    KBD_KEY_O,
    KBD_KEY_P, 
    KBD_KEY_LBRACKET, 
    KBD_KEY_RBRACKET, 
    KBD_KEY_BACK_SLASH, 
    KBD_KP_KEY_7, 
    KBD_KP_KEY_8, 
    KBD_KP_KEY_9, 
    KBD_KP_KEY_PLUS,
    KBD_KEY_CAPS_LOCK, 
    KBD_KEY_A, 
    KBD_KEY_S, 
    KBD_KEY_D, 
    KBD_KEY_F, 
    KBD_KEY_G, 
    KBD_KEY_H, 
    KBD_KEY_J, 
    KBD_KEY_K, 
    KBD_KEY_L, 
    KBD_KEY_SEMICOLON,
    KBD_KEY_APOSTROPH, 
    KBD_KEY_ENTER, 
    KBD_KP_KEY_4, 
    KBD_KP_KEY_5, 
    KBD_KP_KEY_6, 
    KBD_KEY_LSHIFT, 
    KBD_KEY_Z, 
    KBD_KEY_X, 
    KBD_KEY_C,
    KBD_KEY_V, 
    KBD_KEY_B, 
    KBD_KEY_N, 
    KBD_KEY_M, 
    KBD_KEY_COMMA, 
    KBD_KEY_PERIOD, 
    KBD_KEY_SLASH, 
    KBD_KEY_RSHIFT, 
    KBD_KEY_UP, 
    KBD_KP_KEY_1,
    KBD_KP_KEY_2, 
    KBD_KP_KEY_3, 
    KBD_KP_KEY_ENTER, 
    KBD_KEY_LCTRL, 
    KBD_KEY_FN, 
    KBD_KEY_LALT, 
    KBD_KEY_SPACE, 
    KBD_KEY_BACK_SLASH2,
    KBD_KEY_RALT, 
    KBD_KEY_SUPER, 
    KBD_KEY_RCTRL, 
    KBD_KEY_LEFT, 
    KBD_KEY_DOWN, 
    KBD_KEY_RIGHT, 
    KBD_KP_KEY_0, 
    KBD_KP_KEY_PERIOD
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