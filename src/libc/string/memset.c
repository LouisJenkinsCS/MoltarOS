#include <string.h>

void* memset(void* buf, int val, size_t len) {
	unsigned char *b = (unsigned char*) buf;
	
	for(size_t i = 0; i < len; i++)
		b[i] = (unsigned char) val;

	return buf;
}
