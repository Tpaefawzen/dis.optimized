#include <assert.h>
#include <stdio.h>

#include "dis.h"
#include "dis_math.h"

int main(void) {
	dis_int_t testcases[] = {
		1,
		2,
		3,
		11355, /* 0120120120t */
		0
	};

	for ( dis_int_t *i = testcases; *i; *i++ ) {
		dis_int_t x = *i;
		for ( dis_digits_t j = 0; j < DIS_DIGITS; j++ ) {
			printf("%5d ", x);
			x = rotate(DIS_BASE, DIS_DIGITS, x);
		}
		printf("%5d\n", x);
		assert(x == *i);
	}
}
