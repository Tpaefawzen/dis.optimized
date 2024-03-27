#define _POSIX_C_SOURCE 200811L

#include <stdio.h>
#include <stdlib.h>

#include "dis.h"
#include "dis_math.h"

void usage(char *program) {
	fprintf(stderr, "Usage: %s x y\n", program);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	if ( argc < 3 ) usage(argv[0]);

	dis_int_t x, y;
	x = strtoul(argv[1], NULL, 10);
	y = strtoul(argv[2], NULL, 10);

	printf("%d\n", subtract_without_borrow(DIS_BASE, DIS_DIGITS, x, y));
}
