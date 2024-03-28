#define _POSIX_C_SOURCE 200811L

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "dis.h"
#include "dis_math.h"

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
		const dis_digits_t digits,
		const dis_int_t x, const dis_int_t y) {
	switch ( digits ) {
	case 0:
		return 0;

	default:
		/**
		 * Recursiving from bottom digit.
		 */
		/* To avoid overflow on (base + x - y) I had to
		 * do ldiv().rem */
		ldiv_t xdiv = ldiv(x, base);
		ldiv_t ydiv = ldiv(y, base);
		return (base + xdiv.rem - ydiv.rem) % base
			+ base * subtract_without_borrow(
					base, digits-1,
					xdiv.quot, ydiv.quot);
	}
}
