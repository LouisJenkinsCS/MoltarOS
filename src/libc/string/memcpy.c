#include <string.h>

void* memcpy(void* restrict dest, const void* restrict src, size_t len) {
	if (!len) {
		return NULL;
	}
	unsigned char* d = (unsigned char*) dest;
	const unsigned char* s = (const unsigned char*) src;

	for(size_t i = 0; i < len; i++)
		d[i] = s[i];

	return dest;
}
