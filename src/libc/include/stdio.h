#ifndef _STDIO_H
#define _STDIO_H

#include <sys/cdefs.h>

int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
int sprintf(char *out, const char *format, ...);

#endif
