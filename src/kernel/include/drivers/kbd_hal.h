#ifndef MOLTAROS_KBD_HAL_H
#define MOLTAROS_KBD_HAL_H

#include <include/helpers.h>
#include <include/drivers/kbd.h>
#import <stdint.h>

static struct {
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

    // Bit array of currently pressed keys
    uint8_t pressed[BIT_ARRAY_SZ(256)];
} KBD_STATE;

#endif /* endif MOLTAROS_KBD_HAL_H */