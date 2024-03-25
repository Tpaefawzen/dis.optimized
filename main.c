#define _POSIX_C_SOURCE 2

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dis.h"

void usage(const char[]);

void usage(const char myname[]) {
	fprintf(stderr, "Usage: %s FILE\n", myname);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	const char* const myname = argv[0];
	int RESULT = EXIT_FAILURE;

	/**
	 * Usage.
	 */
	for ( int c; ( c = getopt(argc, argv, "")) != -1; ) {
		usage(myname);
	}

	/**
	 * Expect if any argument given.
	 */
	if ( optind >= argc ) {
		usage(myname);
	}

	/**
	 * Regular file?
	 */

	/**
	 * Open a file.
	 */
	const char* const file = argv[optind];
	FILE *f = fopen(file, "r");
	if ( errno ) {
		perror(file);
		errno = 0;
		goto trap_0;
	}

	if ( 0 ) {
trap_0:
		if ( f ) {
			fclose(f);
			if ( errno ) perror(file);
		}

		return RESULT;
	}

	/**
	 *
	 */

	RESULT = EXIT_SUCCESS;
	goto trap_0;
}
