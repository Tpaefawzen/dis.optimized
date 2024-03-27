#include <math.h>
#include <stdint.h>

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
		+ DIS_INT_END(base, digits-1) * bottom_digit;
}

/* FIXME why? */
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
		return (x - y + base) % base
			+ base * subtract_without_borrow(
					base, digits-1, x/base, y/base);
	}
}
