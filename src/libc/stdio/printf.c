#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include <string.h>

// Implementation of itoa()
void int_to_str(long num, int base)
{
	char str[100];
    int i = 0;
    bool isNegative = false;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        putchar('0');
        return;
    }

    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (char)(rem-10) + 'A' : (char) rem + '0';
        num = num/base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    for(int j = strlen(str) - 1; j >= 0; j--)
    	putchar(str[j]);
}

static inline void print(const char* data, size_t len) {
	if(!len)
		len = strlen(data);

	for(size_t i = 0; i < len; i++ )
		putchar(data[i]);
}

/*
	Lazy implementation, defaults to hexadecimal to two places
*/
static inline void hex_to_string(int hex) {
	char buf[10];
	size_t index = 9;

	print("0x", 0);

	while(hex % 16) {
		int num = hex % 16;
		int c;

		// If remainder greater than 10, convert into character [A-F]
		if(num >= 10)
			c = 'A' + (num - 10);
		else
			c = '0' + num;

		buf[index--] = (char) c;
		hex /= 16;
	}

	print(buf + index + 1, (9 - index));
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
			case 'x':
				print("0x", 0);
				int_to_str(va_arg(params, int), 16);
				break;
			case 'd':
				int_to_str(va_arg(params, long), 10);
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
