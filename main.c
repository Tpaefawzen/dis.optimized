#define _POSIX_C_SOURCE 200811L

#include <fcntl.h>
#include <sys/stat.h>

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dis.h"
#include "dis_errno.h"

void usage(const char[]);

void usage(const char myname[]) {
	fprintf(stderr, "Usage: %s [-v] FILE\n", myname);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	const char* const myname = argv[0];
	int result = EXIT_FAILURE;

	_Bool flag_v = 0;

	/**
	 * Usage.
	 */
	for ( int c; ( c = getopt(argc, argv, "v")) != -1; ) {
		switch ( c ) {
		case 'v':
			flag_v = 1;
			break;

		default:
			usage(myname);
		}
	}

	/**
	 * Expect if any argument given.
	 */
	if ( optind >= argc ) {
		usage(myname);
	}
	const char* const filename = argv[optind];

	/**
	 * Open a file.
	 * Parse a Dis program.
	 */
	struct dis_t machine;
	const enum dis_syntax_error syntax_errno =
		dis_compile(filename, &machine);

	if ( 0 ) {
trap_1:
		dis_free(&machine);
		return result;
	}

	if ( errno ) perror(filename);
	errno = 0;

	switch ( syntax_errno ) {
	case DIS_SYNTAX_OK:
		break;

	default:
		fprintf(stderr, "%s:%zu:%zu %s\n",
				filename,
				dis_compilation_lineno, dis_compilation_colno,
				get_dis_syntax_error_msg(syntax_errno));

		goto trap_1;
	}

	if ( flag_v ) {
		machine.flags |= DIS_FLAG_VERBOSE;
	}

	/**
	 * Now we can execute the compiled program.
	 */
	(void)dis_exec_forever(&machine);

	result = EXIT_SUCCESS;
	goto trap_1;
}
