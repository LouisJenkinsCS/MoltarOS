#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include <string.h>

static inline void print(const char* data, size_t len) {
	if(!len)
		len = strlen(data);

	for(size_t i = 0; i < len; i++ )
		putchar(data[i]);
}

int printf(const char * restrict format, ...) {
	va_list params;
	va_start(params, format);

	int index = 0, look_ahead = 0;
	char c;

	while((c = format[index])) {
		// Advance until we find the format specifier token
		if(c != '%') {
			look_ahead = index + 1;

			while(format[look_ahead] && format[look_ahead] != '%')
				look_ahead++;

			// We found our token specifier, write it now
			print(format + index, (size_t) (look_ahead - index));

			index = look_ahead;
			continue;
		}

		/*
			When we finally locate our format token, '%' we automatically look ahead by one
			character to the actual type specifier, I.E '%s', 's' is the type specifier for string.
			The arguments are parsed directly from the va_list and printed, as there currently is no
			actual buffer yet dynamic in size enough to store it.
		*/
		switch(format[++index]) {
			case 'c':
				putchar((char) va_arg(params, int));
				break;
			case 's':
				print(va_arg(params, const char *), 0);
				break;
			case '%':
				// Double format tokens, '%%' evaluate to one '%'
				putchar('%');
				break;
			default:
				// If it is an invalid specifier, just print it out...
				putchar('%');
				putchar(c);

				break;
		}

		index++;
	}

	va_end(params);

	return index;
}
