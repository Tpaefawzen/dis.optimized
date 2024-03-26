#ifndef _DIS_H
#define _DIS_H

#include <stdint.h>

#include "dis_errno.h"

typedef uint16_t dis_int_t;
typedef uint16_t dis_addr_t;
typedef uint8_t dis_base_t;
typedef uint8_t dis_digits_t;


enum default_dis_data_type {
	DIS_BASE = 3,
	DIS_DIGITS = 10,
};

struct dis_t {
	dis_base_t base;
	dis_digits_t digits;
	
	dis_int_t *mem;
	dis_addr_t mem_capacity;

	dis_addr_t source_len;
	dis_addr_t end_nonzero;

	struct {
		dis_int_t a, c, d;
	} reg;

	enum dis_halt_status status;
};

int dis_init(struct dis_t*); /* returns errno */
void dis_free(struct dis_t*);

extern size_t dis_compilation_lineno, dis_compilation_colno;
enum dis_syntax_error dis_compile(const char*const, struct dis_t*);

enum dis_halt_status dis_exec(long);
enum dis_halt_status dis_step(void);

#endif /* _DIS_H */
