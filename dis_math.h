#ifndef _DIS_MATH_H
#define _DIS_MATH_H

#include <stddef.h>
#include <math.h>

#include "dis.h"

/**
 * Maximum integer represented in given base and digits.
 */
dis_addr_t DIS_INT_MAX(const dis_base_t base, const dis_digits_t digits);

/**
 * DIS_INT_MAX() + 1; pow(base, digits)
 */
dis_addr_t DIS_INT_END(const dis_base_t base, const dis_digits_t digits);

/**
 * Move lowest digit to top while shifting other digits to right in specified base.
 */
dis_int_t rotate(const dis_base_t, const dis_digits_t, const dis_int_t);

/**
 * 1. Represent x and y with given base.
 * 2. For each digit in x and y, subtract one from another as given base without borrow.
 * 3. Return the result.
 */
dis_int_t subtract_without_borrow(const dis_base_t, dis_digits_t,
		dis_int_t, dis_int_t);

/**
 * Addition in human math rule. Wrap around.
 */
dis_addr_t dis_addr_add(
		const dis_base_t base, const dis_digits_t digits,
		const dis_addr_t x, const dis_addr_t y);

#endif /* _DIS_MATH_H */
