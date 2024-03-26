#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "dis.h"
#include "dis_errno.h"

int dis_init(struct dis_t* machine) {
	machine -> base = DIS_BASE;
	machine -> digits = DIS_DIGITS;

	machine -> mem_capacity = (size_t)pow(machine->base, machine->digits);
	if ( errno ) {
		return errno;
	}

	machine -> mem = (dis_int_t*)calloc(machine->mem_capacity, sizeof(dis_int_t));
	if ( errno ) {
		return errno;
	}

	machine -> source_len = 0;
	machine -> end_nonzero = 0;

	machine -> reg.a = 0;
	machine -> reg.c = 0;
	machine -> reg.d = 0;
	machine -> status = DIS_RUNNING;

	return errno;
}

void dis_free(struct dis_t *machine) {
	if ( machine->mem_capacity )
		free(machine->mem);
	machine->mem_capacity = 0;
}

enum dis_syntax_error dis_compile(const char*const filename, struct dis_t *machine) {
	int c;
	enum dis_syntax_error result;

	dis_init(machine);
	if ( errno ) return DIS_SYNTAX_MAX;

	FILE *f = fopen(filename, "r");
	if ( !f || errno ) {
		return DIS_SYNTAX_IO;
	}

	if ( 0 ) {
finally:
		fclose(f);
		return result;
	}

not_in_comment:
	switch ( c = fgetc(f) ) {
	case EOF:
		result = DIS_SYNTAX_OK;
		goto finally;

	case ' ': case '\t': case '\n':
		goto not_in_comment;

	default:
		result = DIS_SYNTAX_NON_COMMAND;
		goto finally;

	case '(':
		goto is_in_comment;

	case '!': case '*': case '>': case '^':
	case '_': case '{': case '|': case '}':
		if ( machine->source_len >= machine->mem_capacity ) {
			result = DIS_SYNTAX_TOO_LONG;
			goto finally;
		}

		machine->mem[machine->source_len++] = (dis_int_t)c;
		goto not_in_comment;
	}

is_in_comment:
	switch ( c = fgetc(f) ) {
	case EOF:
		result = DIS_SYNTAX_UNCLOSED_COMMENT;
		goto finally;

	case ')':
		goto not_in_comment;

	default:
		goto is_in_comment;
	}
}
