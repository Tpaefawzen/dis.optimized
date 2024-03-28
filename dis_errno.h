#ifndef _DIS_ERRNO_H
#define _DIS_ERRNO_H

enum dis_syntax_error {
	DIS_SYNTAX_OK = 0,
	DIS_SYNTAX_NON_COMMAND,
	DIS_SYNTAX_UNCLOSED_COMMENT,
	DIS_SYNTAX_TOO_LONG,
	DIS_SYNTAX_IO,
	DIS_SYNTAX_MEMORY,
	DIS_SYNTAX_MAX,
};

enum dis_halt_status {
	DIS_RUNNING = 0,
	DIS_HALT_COMMAND,
	DIS_HALT_OUTPUT_EOF,
	DIS_HALT_STATUS_MAX,
};

const char *get_dis_syntax_error_msg(enum dis_syntax_error);
const char *get_dis_halt_status_msg(enum dis_halt_status);

#endif /* _DIS_ERRNO_H */
