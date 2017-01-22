#include <include/mm/zone.h>

extern uint32_t PHYSICAL_MEMORY_START;
extern uint32_t PHYSICAL_MEMORY_END;

void zone_init() {
	// The page frame at 4MBs was reserved for us. We need to chunk it into 4KB aligned
	// and sized chunks.
}