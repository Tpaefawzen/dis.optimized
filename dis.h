#ifndef _DIS_H
#define _DIS_H

#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef uint16_t dis_int_t;
typedef uint16_t dis_addr_t;
typedef uint8_t dis_base_t;
typedef uint8_t dis_digits_t;

#include "dis_errno.h"

enum default_dis_data_type {
	DIS_BASE = 3,
	DIS_DIGITS = 10,
};

enum dis_flag_t {
	DIS_FLAG_VERBOSE = 1 << 0,
};

struct dis_t {
	dis_base_t base;
	dis_digits_t digits;
	
	dis_int_t *mem;
	dis_addr_t mem_capacity;

	dis_addr_t source_len;

	dis_addr_t begin_nonnop;
	dis_addr_t end_nonnop;

	dis_addr_t *next_nonnop;

	struct {
		dis_int_t a, c, d;
	} reg;

	enum dis_halt_status status;

	enum dis_flag_t flags;

	volatile sig_atomic_t caught_signal_number;

	FILE *fin;
	FILE *fout;
};

int dis_init(struct dis_t*); /* returns errno */
void dis_free(struct dis_t*);

extern size_t dis_compilation_lineno, dis_compilation_colno;
enum dis_syntax_error dis_compile(const char*const, struct dis_t*, _Bool);

enum dis_halt_status dis_exec(struct dis_t*, size_t);
enum dis_halt_status dis_exec_forever(struct dis_t*);
enum dis_halt_status dis_step(struct dis_t*);

void DPRINTF(struct dis_t*, const char *restrict, ...);

#endif /* _DIS_H */
