#include "dis_errno.h"

const char *const dis_syntax_error_message[DIS_SYNTAX_MAX] = {
	"Syntax OK",
	"Not a command",
	"Comment unclosed",
	"Program too long",
	"I/O error",
};

const char *const dis_halt_status_message[DIS_HALT_STATUS_MAX] = {
	"Is running",
	"Halt by halt command",
	"Halt by writing EOF value",
};

const char *get_dis_syntax_error_msg(enum dis_syntax_error number) {
	if ( 0 <= number && number < DIS_SYNTAX_MAX ) {
		return dis_syntax_error_message[number];
	}
	return "Unknown syntax error";
}

const char *get_dis_halt_status_msg(enum dis_halt_status number) {
	if ( 0 <= number && number < DIS_HALT_STATUS_MAX ) {
		return dis_halt_status_message[number];
	}
	return "Unknown halt status";
}

