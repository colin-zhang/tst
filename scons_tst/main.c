#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int log_print(char *fmt, ...);

int main(int argc, char *argv[]) {
	int ret;
	log_print("Hello world\n");
	return 0;
}
