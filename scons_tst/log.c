#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

int log_print(char *fmt, ...) {
	
	va_list args;
	va_start(args, fmt);
	vfprintf(stdout, fmt, args);
	fflush(stdout);
	va_end(args);

}
