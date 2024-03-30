#ifndef _DIS_MATH_H
#define _DIS_MATH_H

#include <math.h>

#include "dis.h"

inline dis_int_t DIS_INT_END(const dis_base_t base, const dis_digits_t digits) {
	return (dis_int_t)pow(base, digits);
}

inline dis_int_t DIS_INT_MAX(const dis_base_t base, const dis_digits_t digits) {
	return DIS_INT_END(base, digits) - 1;
}

dis_int_t rotate(const dis_base_t, const dis_digits_t, const dis_int_t);
dis_int_t subtract_without_borrow(const dis_base_t, dis_digits_t,
		dis_int_t, dis_int_t);

inline dis_addr_t dis_addr_add(
		const dis_base_t base, const dis_digits_t digits,
		const dis_addr_t x, const dis_addr_t y) {
	const dis_addr_t endval = DIS_INT_END(base, digits);
	return (dis_addr_t)(
			((uintptr_t)x + (uintptr_t)y - endval)
			% endval);
}

#endif /* _DIS_MATH_H */
