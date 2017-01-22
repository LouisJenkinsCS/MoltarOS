#ifndef MOLTAROS_ZONE_H
#define MOLTAROS_ZONE_H

#include <stdint.h>

void zone_init();

uint32_t zone_alloc(int flags);

#endif