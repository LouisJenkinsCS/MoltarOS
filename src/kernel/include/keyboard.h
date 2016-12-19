#ifndef MOLTAROS_KEYBOARD_H
#define MOLTAROS_KEYBOARD_H

#define KBD_PORT 0x60
#define RELEASE_MASK 0x80

#import <stdint.h>

struct scan_code {
  uint8_t normal;
  uint8_t escaped;  
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