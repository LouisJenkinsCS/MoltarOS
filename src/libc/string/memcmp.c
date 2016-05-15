#include <string.h>

int memcmp(const void *first, const void *second, size_t len) {
	const unsigned char *a = (const unsigned char *) first;
	const unsigned char *b = (const unsigned char *) second;

	for(size_t i = 0; i < len; i++)
		if(a[i] < b[i])
			return -1;
		else if(b[i] < a[i])
			return 1;

	return 0;
}
