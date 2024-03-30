#define _POSIX_C_SOURCE 200811L

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "dis.h"
#include "dis_math.h"

dis_addr_t DIS_INT_MAX(const dis_base_t base, const dis_digits_t digits) {
	return (dis_int_t)pow((double)base, (double)digits);
}

dis_addr_t DIS_INT_END(const dis_base_t base, const dis_digits_t digits) {
	return DIS_INT_MAX(base, digits)-1;
}

dis_addr_t dis_addr_add(
		const dis_base_t base, const dis_digits_t digits,
		const dis_addr_t x, const dis_addr_t y) {
	const dis_addr_t endval = DIS_INT_END(base, digits);
	return (dis_addr_t)(
			((ptrdiff_t)x + (ptrdiff_t)y - endval)
			% endval);
}

dis_int_t rotate(const dis_base_t base, const dis_digits_t digits,
		const dis_int_t x) {
	/**
	 * abcdefgh -> habcdefg(3)
	 *  abcdefg = x / 3;
	 *        h = x % 3
	 *        h = h * pow(3, 0)
	 *       h0 = h * pow(3, 1)
	 *      h00 = h * pow(3, 2)
	 *     h000 = h * pow(3, 3)
	 * h0000000 = h * pow(3, 7)
	 * habcdefg = h0000000 + abcdefg
	 */
	const dis_int_t bottom_digit = x % base;
	const dis_int_t nonbottom_shift_right = (dis_int_t)(x / base);
	const dis_digits_t left_shift_for = digits - 1;

	return nonbottom_shift_right
		+ DIS_INT_END(base, left_shift_for) * bottom_digit;
}

dis_int_t subtract_without_borrow(const dis_base_t base,
		dis_digits_t digits,
		dis_int_t x, dis_int_t y) {
	dis_int_t result = 0, weight = 1;
	for ( ; digits--; weight*=base ) {
		/**
		 * Recursiving from bottom digit.
		 */
		/* To avoid overflow on (base + x - y) I had to
		 * do ldiv().rem */
		ldiv_t xdiv = ldiv(x, base);
		ldiv_t ydiv = ldiv(y, base);
		result += weight
			* ((base + xdiv.rem - ydiv.rem) % base);
		x = xdiv.quot, y = ydiv.quot;
	}
	
	return result;
}
