#define _POSIX_C_SOURCE 200811L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dis.h"
#include "dis_fmt.h"

char *dis_dec2trit(char *result, dis_int_t x, const size_t n_result, _Bool as_fixed) {
	/**
	 * Using bc XXX any other ideas?
	 */
	const char cmd_template[] = "echo 'obase=3;%d'|bc";
	char actual_command[20 + 5 + 1];
	snprintf(actual_command, 20+5, cmd_template, x%DIS_BASE);
	FILE *f = popen(actual_command, "r");
	if ( ! f || errno ) {
		return NULL;
	}

	char local_result[DIS_DIGITS+1];
	int could_read_some = fscanf(f, "%10s", &local_result);
	fclose(f);

	if ( as_fixed ) {
		long dec_packed_trit = strtol(local_result, NULL, 10);
		snprintf(local_result, 10, "%010d", dec_packed_trit);
	}
	
	return strncpy(result, local_result, n_result);
}

dis_int_t dis_trit2dec(const char* const trit_str) {
	return (dis_int_t)strtol(trit_str, NULL, 3);
}

#if 0

char *dis_dec2trit(char *result, dis_int_t x, const size_t n_result, _Bool as_fixed) {
	static char local_result[DIS_DIGITS+1] = { 0 };
	const char to_digit[DIS_BASE] = { '0', '1', '2' };

	dis_digits_t i;
	for ( i = DIS_DIGITS; i-- > 0; ) {
		local_result[i] = to_digit[(int)x % (int)DIS_BASE];
		x /= DIS_BASE;
		if ( ! as_fixed && ! x ) break;
	}

	if ( as_fixed ) return strncpy(result, local_result, n_result);
	i++;
	return strncpy(result, local_result+1, n_result);

}

dis_int_t dis_trit2dec(const char *restrict trits) {
	dis_int_t result = 0;
	for ( dis_digits_t i = 0; i < DIS_DIGITS; i++ ) {
		result *= DIS_BASE;
		switch(trits[i]) {
		case '0':
			result += 0;
			break;

		case '1':
			result += 1;
			break;

		case '2':
			result += 2;
			break;

		default:
			return result;
			result += 0;
		}
	}

	return result;
}

#endif
