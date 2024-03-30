#define _POSIX_C_SOURCE 200811L

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dis.h"
#include "dis_errno.h"
#include "dis_math.h"

volatile sig_atomic_t has_caught_signal_ = 0;

static void handler(int signum) {
	has_caught_signal_ = signum;
}

/* Not even macro. */
inline _Bool dis_is_infinite_loop(const struct dis_t *machine) {
	return machine->status == DIS_RUNNING &&
		machine->end_nonnop == 0;
}

inline dis_int_t DIS_T_INT_MAX(const struct dis_t *const machine) {
	return machine->mem_capacity;
}

/* Methods. */
int dis_init(struct dis_t* machine) {
	machine -> mem = NULL;

	machine -> base = DIS_BASE;
	machine -> digits = DIS_DIGITS;

	machine -> mem_capacity =
		DIS_INT_END(machine->base, machine->digits);
	if ( errno ) return errno;

	machine -> mem =
		(dis_int_t*)calloc(DIS_T_INT_MAX(machine), sizeof(dis_int_t));
	if ( errno ) return errno;

	int sigs_[] = { SIGHUP, SIGINT, SIGQUIT, SIGPIPE, SIGALRM, SIGTERM, 0 };

	for ( int *i = sigs_;
			*i;
			i++ ) {
		if ( signal(*i, handler) == SIG_ERR ) {
			fprintf(stderr, "dis_init(): "
					"Failed to signal(%s, handler): "
					"%s\n",
					strsignal(*i),
					strerror(errno));
			errno = 0;
		}
	}

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
	if ( DIS_T_INT_MAX(machine) && machine->mem )
		free(machine->mem);
	machine->mem = NULL;
	machine->mem_capacity = 0;
}

size_t dis_compilation_lineno;
size_t dis_compilation_colno;
void increment_lineno_or_colno_(const int);
_Bool accept_any_char_for_source = 0;

enum dis_syntax_error parse_non_comment_(FILE*, struct dis_t*);
enum dis_syntax_error parse_comment_(FILE*, struct dis_t*);
void extend_nonnop_at_compilation_(struct dis_t*);

enum dis_syntax_error dis_compile(
		const char*const filename, struct dis_t *machine,
		_Bool accept_any_char) {
	dis_compilation_lineno = 1;
	dis_compilation_colno = 0;
	accept_any_char_for_source = accept_any_char;

	dis_init(machine);
	if ( errno ) return DIS_SYNTAX_MEMORY;

	FILE *f = fopen(filename, "r");
	if ( !f || errno ) {
		return DIS_SYNTAX_IO;
	}

	enum dis_syntax_error result = parse_non_comment_(f, machine);
	if ( result == DIS_SYNTAX_OK ) {
		/* maybe TODO: flag to accept or reject I/O error when syntax is ok */
		if ( ferror(f) )
			result = DIS_SYNTAX_IO;
	}

	fclose(f);
	return result;
}

size_t comment_line, comment_col;

enum dis_syntax_error parse_non_comment_(FILE *f, struct dis_t *machine) {
	if ( has_caught_signal_ ) {
		machine->caught_signal_number = has_caught_signal_;
		return DIS_SYNTAX_MAX;
	}

	int c;
	increment_lineno_or_colno_(c = fgetc(f));
	switch ( c ) {
	case EOF:
		return DIS_SYNTAX_OK;

	case ' ': case '\t': case '\n':
		return parse_non_comment_(f, machine);

	case '(':
		comment_line = dis_compilation_lineno;
		comment_col = dis_compilation_colno;
		return parse_comment_(f, machine);

	default:
		if ( ! accept_any_char_for_source )
			return DIS_SYNTAX_NON_COMMAND;
		/* FALLTHROUGH */

	case '!': case '*': case '>': case '^':
	case '_': case '{': case '|': case '}':
		if ( machine->source_len >= DIS_T_INT_MAX(machine) ) {
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
		machine->end_nonnop = machine->source_len;
	}
}

enum dis_syntax_error parse_comment_(FILE *f, struct dis_t *machine) {
	if ( has_caught_signal_ ) {
		machine->caught_signal_number = has_caught_signal_;
		return DIS_SYNTAX_MAX;
	}

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

/* Methods to do something with compiled program. */
enum dis_halt_status dis_exec(struct dis_t* machine, size_t steps) {
	for ( ; ; ) {
		if ( has_caught_signal_ ) {
			machine->caught_signal_number = has_caught_signal_;
			return machine->status;
		}
		if ( ! steps ) return machine->status;
		if ( machine->status != DIS_RUNNING ) return machine->status;
		dis_step(machine);
		steps--;
	}
}

enum dis_halt_status dis_exec_forever(struct dis_t* machine) {
	for ( ; ; ) {
		if ( has_caught_signal_ ) {
			machine->caught_signal_number = has_caught_signal_;
			return machine->status;
		}
		if ( machine->status != DIS_RUNNING ) return machine->status;
		dis_step(machine);
	}
}

typedef enum dis_halt_status cmd_f(struct dis_t*);
cmd_f halt_, jmp_or_load_, rot_or_opr_, out_, in_;
cmd_f *fetch_cmd_(const dis_int_t);

enum dis_halt_status dis_step(struct dis_t* machine) {
	if ( has_caught_signal_ ) {
		machine->caught_signal_number = has_caught_signal_;
		return machine->status;
	}

	/* Step 1. Reject halt machine. */
	if ( machine->status != DIS_RUNNING ) {
		return machine->status;
	}

	/* Step 2. Increment c and d until mem[c] is a non-nop if any. */
try_to_fetch_command:
	if ( has_caught_signal_ ) {
		machine->caught_signal_number = has_caught_signal_;
		return machine->status;
	}

	if ( dis_is_infinite_loop(machine) )
		return machine->status;

	if ( machine->reg.c >= machine->end_nonnop ) {
		/* Since command can be found at <end_nonnop, why not just
		 * increment c and d until c is 0? */
		DPRINTF(machine, "c %u >= end_nonnop %u\n",
				machine->reg.c, machine->end_nonnop);
		machine->reg.d = (uintptr_t)(
				machine->reg.d + machine->reg.c
				+ DIS_T_INT_MAX(machine) )
			% DIS_T_INT_MAX(machine);
		machine->reg.c = 0;
		goto try_to_fetch_command;
	}

	/* Are there any command in [c, end_nonnop)? */
	cmd_f *cmd = NULL;
	for ( ;
			machine->reg.c < machine->end_nonnop;
	    ) {
		if ( has_caught_signal_ ) {
			machine->caught_signal_number = has_caught_signal_;
			return machine->status;
		}
		if ( ( cmd = fetch_cmd_(machine->mem[machine->reg.c]) ) )
			goto found_cmd;
		machine->reg.c = ( machine->reg.c + 1 ) % DIS_T_INT_MAX(machine);
		machine->reg.d = ( machine->reg.d + 1 ) % DIS_T_INT_MAX(machine);
	}

	/* Command not found, so it can be found at [0, c). */
	assert( machine->reg.c >= machine->end_nonnop );
	machine->end_nonnop = machine->reg.c;
	DPRINTF(machine, "end_nonnop: %u\n", machine->end_nonnop);
	goto try_to_fetch_command;

found_cmd:
	DPRINTF(machine, "a %5d c %5d d %5d mem[c] %5d mem[d] %5d\n",
			machine->reg.a,
			machine->reg.c,
			machine->reg.d,
			machine->mem[machine->reg.c],
			machine->mem[machine->reg.d]);

	/* Step 3. Execute a fetched non-nop.
	 * OBTW '|' and '>' can extend or shrink end_nonnop. */
	cmd(machine);
	switch ( machine->status ) {
	case DIS_RUNNING:
		break;

	default:
		return machine->status;
	}

	DPRINTF(machine, "a %5d c %5d d %5d mem[c] %5d mem[d] %5d\n",
			machine->reg.a,
			machine->reg.c,
			machine->reg.d,
			machine->mem[machine->reg.c],
			machine->mem[machine->reg.d]);

	/* Step 4. Increment c and d for next step. */
	machine->reg.c = ( machine->reg.c + 1 ) % DIS_T_INT_MAX(machine);
	machine->reg.d = ( machine->reg.d + 1 ) % DIS_T_INT_MAX(machine);

	return machine->status;
}

#define DIS_INT_T_MAX(machine) ((machine)->mem_capacity)

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

/* non-nop commands. */

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

enum dis_halt_status rot_or_opr_(struct dis_t *machine) {
	/**
	 * This command shall modify mem[d], which may result in
	 * either command or not.
	 */
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

	/* Extend or shrink end_nonnop */
	if ( machine->reg.d < machine->end_nonnop )
		goto finally;
	if ( machine->reg.d == machine->end_nonnop ) {
		if ( fetch_cmd_(x) )
			;
		else
			machine->end_nonnop = machine->reg.d;
		goto finally;
	}
	if ( machine->reg.d > machine->end_nonnop ) {
		if ( fetch_cmd_(x) )
			machine->end_nonnop = machine->reg.d + 1;
		else
			;
		goto finally;
	}
	/* NOTREACHED */
	assert( ! "machine->reg.d not <end_nonnop, =0, nor >end_nonnop" );

finally:
	return machine->status;
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
		machine->reg.a = (dis_int_t)x;
	}
	return machine->status;
}
