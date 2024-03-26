#define _POSIX_C_SOURCE 200809L

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

_Bool is_regular_file(const char[]);
void usage(const char[]);

void usage(const char myname[]) {
	fprintf(stderr, "Usage: %s FILE\n", myname);
	exit(EXIT_FAILURE);
}

_Bool is_regular_file(const char path[]) {
	struct stat statbuf;
	_Bool result = 0;

	switch ( lstat(path, &statbuf) ) {
	case -1: default:
		if ( errno ) perror(path);
		errno = 0;
		break;

	case 0:
		result = S_ISREG(statbuf.st_mode);
		return result;
	}

lstat_failed_wtf:
	switch ( errno ) {
	case EACCES:
	case EIO:
	case ELOOP:
	case ENAMETOOLONG:
	case ENOENT:
	case ENOTDIR:
	case EOVERFLOW:
		perror(path);
		return 0;
	default:
		perror(path);
		return 0;
	}
}

int main(int argc, char *argv[]) {
	const char* const myname = argv[0];
	int result = EXIT_FAILURE;

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
	const char* const filename = argv[optind];

#if 0
	/**
	 * Regular file?
	 */
	if ( ! is_regular_file(filename) ) {
		fprintf(stderr, "%s: Rejected due to not regular file\n", filename);
		return result;
	}
#endif

	/**
	 * Open a file.
	 * Parse a Dis program.
	 */
	struct dis_t machine;
	enum dis_syntax_error syntax_errno = dis_compile(filename, &machine);

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
		const char *const errmsg = get_dis_syntax_error_msg(syntax_errno);
		fprintf(stderr, "%s:%zu:%zu %s\n", filename, dis_compilation_lineno, dis_compilation_colno, errmsg);

		goto trap_1;
	}

	/**
	 * Now we can execute the compiled program.
	 */

	result = EXIT_SUCCESS;
	goto trap_1;
}
