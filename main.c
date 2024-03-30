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
	fprintf(stderr, "Usage: %s [-Ev] [-O LEVEL] FILE\n", myname);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	const char* const myname = argv[0];
	int result = EXIT_FAILURE;

	_Bool flag_E = 0;
	_Bool flag_v = 0;
	uint8_t flag_Optlevel = 3;

	/**
	 * Usage.
	 */
	for ( int c; ( c = getopt(argc, argv, "EvO:")) != -1; ) {
		switch ( c ) {
			unsigned long maybe_optlevel_;
			char *endptr_;

		case 'E':
			flag_E = 1;
			break;

		case 'v':
			flag_v = 1;
			break;

		case 'O':
			maybe_optlevel_ = strtoul(optarg, &endptr_, 10);
			if ( errno ) {
				perror("-O");
				errno = 0;
			}
			if ( maybe_optlevel_ > 100 ) {
				fprintf(stderr, "-O %lu: optimization level too large\n",
						maybe_optlevel_);
				maybe_optlevel_ = flag_Optlevel;
			}
			if ( endptr_ == optarg ) {
				fprintf(stderr, "-O %s: not a number\n",
						optarg);
				break;
			}
			flag_Optlevel = maybe_optlevel_;

		default:
		case '?':
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
		dis_compile(filename, &machine, flag_E);

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

	result = machine.caught_signal_number;
	if ( result ) result += 128;
	goto trap_1;
}
