#include <stdio.h>
#include <string.h>

#include "dis.h"
#include "dis_fmt.h"

char *dis_decimal2fixed_trit(char *result, dis_int_t x, const size_t n_result) {
	char local_result[DIS_DIGITS+1] = { 0 };
	const char to_digit[DIS_BASE] = { '0', '1', '2' };

	for ( dis_digits_t i = DIS_DIGITS; i-- > 0; ) {
		local_result[i] = to_digit[(int)x % (int)DIS_BASE];
		x /= DIS_BASE;
	}

	return strncpy(result, local_result, n_result);
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
