#define _POSIX_C_SOURCE 200801L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dis.h"
#include "dis_fmt.h"

void usage(char *name) {
	fprintf(stderr, "Usage: %s -d DECIMAL\n", name);
	fprintf(stderr, "Usage: %s -t TRITS\n",   name);
	exit(EXIT_FAILURE);
}

void decimal2trit(char *decimal_str) {
	long x = strtol(decimal_str, NULL, 10);
	abort(); /* TODO */
	char result[DIS_BASE+1];
	printf("%s\n", dis_dec2trit(result,
				strtol(decimal_str, NULL, 10),
				DIS_BASE,
				/*as_fixed*/ 1));
}

void trit2decimal(char *trit_str) {
	printf("%05d\n", strtol(trit_str, NULL, 3));
}

int main(int argc, char *argv[]) {
	if ( argc < 3 ) {
		usage(argv[0]);
	}

	_Bool input_is_decimal = 1;
	for ( int c; (c = getopt(argc, argv, "dt")) != -1; ) {
	switch ( c ) {
	case 'd':
		input_is_decimal = 1;
		break;
	case 't':
		input_is_decimal = 0;
		break;

	default:
		usage(argv[0]);
	}
	}

	if ( input_is_decimal ) {
		decimal2trit(argv[optind]);
		return EXIT_SUCCESS;
	} else {
		trit2decimal(argv[optind]);
		return EXIT_SUCCESS;
	}
}
