#ifndef _DIS_H
#define _DIS_H

#define POSIXLY_CORRECT 1
#define _POSIX_C_SOURCE 2

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

enum {
	DIS_BASE = 3,
	DIS_DIGITS = 10,
};

typedef uint16_t dis_int_t;
typedef uint16_t dis_addr_t;

#define DIS_MEM_SIZE (DIS_BASE*DIS_DIGITS)

enum dis_syntax_error {
	DIS_SYNTAX_OK = 0,
	DIS_SYNTAX_NON_COMMAND,
	DIS_SYNTAX_UNCLOSED_COMMENT,
	DIS_SYNTAX_TOO_LONG,
	DIS_SYNTAX_MAX,
};

const char *const dis_syntax_error_string[DIS_SYNTAX_MAX] = {
	"Syntax OK",
	"Not a command",
	"Comment unclosed",
	"Program too long",
};

enum dis_syntax_error dis_compile(FILE*);

enum dis_halt_status {
	DIS_RUNNING = 0,
	DIS_HALT_COMMAND,
	DIS_HALT_OUTPUT_EOF,
	DIS_HALT_STATUS_MAX,
};

const char *const dis_halt_status_message[DIS_HALT_STATUS_MAX] = {
	"Is running",
	"Halt by halt command",
	"Halt by writing EOF value",
};

dis_int_t mem[DIS_MEM_SIZE] = { 0 };
_Bool is_modified[DIS_MEM_SIZE] = { 0 };
dis_addr_t last_nonzero = 0;

enum dis_halt_status dis_exec(long);
enum dis_halt_status dis_step(void);



#endif /* _DIS_H */
