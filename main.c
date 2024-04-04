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
void parse_args_(int argc, char *argv[]);

const char *myname;

_Bool flag_E = 0;
_Bool flag_v = 0;
_Bool flag_k = 0; unsigned long long steps2run;
uint8_t flag_Optlevel = 3;

void usage(const char myname[]) {
	fprintf(stderr, "Usage: %s [-Ev] [-k STEPS] [-O LEVEL] FILE\n", myname);
	exit(EXIT_FAILURE);
}

void parse_args_(int argc, char *argv[]) {
	for ( int c; ( c = getopt(argc, argv, "Evk:O:")) != -1; ) {
		switch ( c ) {
			unsigned long maybe_optlevel_;
			char *endptr_;

		case 'E':
			flag_E = 1;
			break;

		case 'v':
			flag_v = 1;
			break;

		case 'k':
			flag_k = 1;
			steps2run = strtoull(optarg, &endptr_, 10);
			if ( errno ) {
				perror("-k");
				fprintf(stderr, "-k %s: Treated as %llu\n",
						optarg, steps2run);
				errno = 0;
			}
			if ( endptr_ == optarg ) {
				fprintf(stderr, "-k %s: not a number\n",
						optarg);
				break;
			}
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
			break;

		default:
		case '?':
			usage(myname);
		}
	}

}

int main(int argc, char *argv[]) {
	myname = argv[0];
	int result = EXIT_FAILURE;

	parse_args_(argc, argv);

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
	dis_init(&machine);
	if ( flag_v ) machine.flags |= DIS_FLAG_VERBOSE;

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

	/**
	 * Now we can execute the compiled program.
	 */
	if ( flag_k ) {
		enum dis_halt_status halt_status = dis_exec(&machine, (size_t)steps2run);
		result = ! halt_status;
	} else {
		(void)dis_exec_forever(&machine);
		result = EXIT_SUCCESS;
	}

	/**
	 * Finally
	 */
	int signal_no = machine.caught_signal_number;
	if ( signal_no ) result = signal_no + 128;

	goto trap_1;
}
