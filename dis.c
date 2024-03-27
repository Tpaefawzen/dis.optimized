#define _POSIX_C_SOURCE 200801L

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "dis.h"
#include "dis_errno.h"
#include "dis_math.h"

inline _Bool dis_is_infinite_loop(const struct dis_t*);

int dis_init(struct dis_t* machine) {
	machine -> base = DIS_BASE;
	machine -> digits = DIS_DIGITS;

	machine -> mem_capacity =
		(size_t)pow(machine->base, machine->digits);
	if ( errno ) return errno;

	machine -> mem =
		(dis_int_t*)calloc(machine->mem_capacity, sizeof(dis_int_t));
	if ( errno ) return errno;

	machine -> source_len = 0;
	machine -> end_nonnop = 0;

	machine -> reg.a = 0;
	machine -> reg.c = 0;
	machine -> reg.d = 0;
	machine -> status = DIS_RUNNING;

	machine -> flags = 0;

	return errno;
}

void dis_free(struct dis_t *machine) {
	if ( machine->mem_capacity )
		free(machine->mem);
	machine->mem_capacity = 0;
}

size_t dis_compilation_lineno;
size_t dis_compilation_colno;
void increment_lineno_or_colno_(const int);

enum dis_syntax_error parse_non_comment_(FILE*, struct dis_t*);
enum dis_syntax_error parse_comment_(FILE*, struct dis_t*);
void extend_nonnop_at_compilation_(struct dis_t*);

enum dis_syntax_error dis_compile(
		const char*const filename, struct dis_t *machine) {
	dis_compilation_lineno = 1;
	dis_compilation_colno = 0;

	enum dis_syntax_error result;

	dis_init(machine);
	if ( errno ) return DIS_SYNTAX_MAX;

	FILE *f = fopen(filename, "r");
	if ( !f || errno ) {
		return DIS_SYNTAX_IO;
	}

	switch ( result = parse_non_comment_(f, machine) ) {
	case DIS_SYNTAX_OK:
		machine->end_nonnop = machine->source_len;
	}

	fclose(f);
	return result;
}

size_t comment_line, comment_col;

enum dis_syntax_error parse_non_comment_(FILE *f, struct dis_t *machine) {
	int c;
	increment_lineno_or_colno_(c = fgetc(f));
	switch ( c ) {
	case EOF:
		return DIS_SYNTAX_OK;

	case ' ': case '\t': case '\n':
		return parse_non_comment_(f, machine);

	default:
		return DIS_SYNTAX_NON_COMMAND;

	case '(':
		comment_line = dis_compilation_lineno;
		comment_col = dis_compilation_colno;
		return parse_comment_(f, machine);

	case '!': case '*': case '>': case '^':
	case '_': case '{': case '|': case '}':
		if ( machine->source_len >= machine->mem_capacity ) {
			return DIS_SYNTAX_TOO_LONG;
		}

		machine->mem[machine->source_len++] = (dis_int_t)c;
		extend_nonnop_at_compilation_(machine);
		return parse_non_comment_(f, machine);
	}
}

void extend_nonnop_at_compilation_(struct dis_t *machine) {
	switch ( machine->mem[machine->source_len-1] ) {
	case '_':
		return;

	default:
	case '!': case '*': case '>': case '^':
	case '{': case '|': case '}':
		machine->end_nonnop = machine->source_len-1;
	}
}

enum dis_syntax_error parse_comment_(FILE *f, struct dis_t *machine) {
	int c;
	increment_lineno_or_colno_(c = fgetc(f));
	switch ( c ) {
	case EOF:
		dis_compilation_lineno = comment_line;
		dis_compilation_colno = comment_col;
		return DIS_SYNTAX_UNCLOSED_COMMENT;

	case ')':
		return parse_non_comment_(f, machine);

	default:
		return parse_comment_(f, machine);
	}
}

void increment_lineno_or_colno_(const int c) {
	switch ( c ) {
	case EOF:
		return;

	case '\n':
		dis_compilation_colno = 1;
		dis_compilation_lineno++;
		return;

	default:
		dis_compilation_colno++;
	}
}

enum dis_halt_status dis_exec(struct dis_t* machine, size_t steps) {
	for ( ; ; ) {
		if ( ! steps ) return machine->status;
		if ( machine->status != DIS_RUNNING ) return machine->status;
		dis_step(machine);
		steps--;
	}	
}

enum dis_halt_status dis_exec_forever(struct dis_t* machine) {
	for ( ; ; ) {
		if ( machine->status != DIS_RUNNING ) return machine->status;
		dis_step(machine);
	}
}

void increment_c_and_d_to_cmd_(struct dis_t*);

typedef enum dis_halt_status cmd_f(struct dis_t*);
cmd_f halt_, jmp_or_load_, rot_or_opr_, out_, in_;
cmd_f *fetch_cmd_(const dis_int_t);

enum dis_halt_status dis_step(struct dis_t* machine) {
	if ( machine->status != DIS_RUNNING ) return machine->status;
	if ( dis_is_infinite_loop(machine) ) return machine->status;

	cmd_f *cmd = fetch_cmd_(machine->mem[machine->reg.c]);
	if ( cmd ) {
		cmd(machine);
		switch ( machine->status ) {
		case DIS_RUNNING:
			break;

		default:
			return machine->status;
		}
	}

	DPRINTF(machine, "a %zu c %zu d %zu mem[c] %zu mem[d] %zu\n",
			machine->reg.a,
			machine->reg.c,
			machine->reg.d,
			machine->mem[machine->reg.c],
			machine->mem[machine->reg.d]);
	increment_c_and_d_to_cmd_(machine);

	return machine->status;
}

#define DIS_INT_T_MAX(machine) ((machine)->mem_capacity)
#define DIS_INT_T_END(machine) ((machine)->mem_capacity+1)

const dis_int_t incr_(const struct dis_t *machine, const dis_int_t x) {
	if ( x == DIS_INT_T_MAX(machine) )
		return 0;
	return x + 1;
}

void skip_to_mem0_(struct dis_t*);

void increment_c_and_d_to_cmd_(struct dis_t* machine) {
	if ( machine->reg.c < machine->end_nonnop ) {
		machine->reg.d = incr_(machine, machine->reg.d);
		machine->reg.c = incr_(machine, machine->reg.c);
		return;
	}

	skip_to_mem0_(machine);
}

void skip_to_mem0_(struct dis_t *machine) {
	uintptr_t new_d_;
	new_d_ = machine->reg.d + machine->reg.c + DIS_INT_T_END(machine);
	new_d_ %= DIS_INT_T_END(machine);
	machine->reg.d = new_d_;
	machine->reg.c = 0;
}

cmd_f *fetch_cmd_(const dis_int_t x) {
	switch ( x ) {
	case '!': return halt_;
       	case '*': case '^': return jmp_or_load_;
	case '>': case '|': return rot_or_opr_;
	case '{': return out_;
	case '}': return in_;
	}
	return (cmd_f*)NULL;
}

enum dis_halt_status halt_(struct dis_t *machine) {
	DPRINTF(machine, "Reached to '!'\n");
	return machine->status = DIS_HALT_COMMAND;
}


enum dis_halt_status jmp_or_load_(struct dis_t *machine) {
	DPRINTF(machine, "Reached to either '^' or '*'\n");
	dis_int_t x = machine->mem[machine->reg.c];
	*( x == '^' ? &machine->reg.c : &machine->reg.d ) =
		machine->mem[machine->reg.d];
	return machine->status;
}

void extend_nonnop_when_mem_modified_(struct dis_t*);

enum dis_halt_status rot_or_opr_(struct dis_t *machine) {
	DPRINTF(machine, "Reached to either '>' or '|'\n");
	dis_int_t x;
	x = machine->mem[machine->reg.c];
	x = machine->reg.a = machine->mem[machine->reg.d] = (
			x =='>' ?
			rotate(machine->base,
				machine->digits,
				machine->mem[machine->reg.d]) :
			subtract_without_borrow(
				machine->base,
				machine->digits,
				machine->reg.a,
				machine->mem[machine->reg.d]));
	extend_nonnop_when_mem_modified_(machine);
	return machine->status;
}

void extend_nonnop_when_mem_modified_(struct dis_t *machine) {
	if ( fetch_cmd_(machine->reg.a) ) {
		machine->end_nonnop = machine->reg.d+1;
	}
}

enum dis_halt_status out_(struct dis_t *machine) {
	DPRINTF(machine, "Reached to {\n");
	if ( machine->reg.a == DIS_INT_T_MAX(machine) ) {
		return machine->status = DIS_HALT_OUTPUT_EOF;
	}
	fputc(machine->reg.a, stdout);
	return machine->status;
}

enum dis_halt_status in_(struct dis_t *machine) {
	DPRINTF(machine, "Reached to }\n");
	int x = fgetc(stdin);
	switch ( x ) {
	case EOF:
		machine->reg.a = DIS_INT_T_MAX(machine);
		break;

	default:
		machine->reg.a = x;
	}
	return machine->status;
}

inline _Bool dis_is_infinite_loop(const struct dis_t *machine) {
	return machine->status == DIS_RUNNING &&
		machine->end_nonnop == 0;
}
