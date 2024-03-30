#ifndef _DIS_MATH_H
#define _DIS_MATH_H

#include <stddef.h>
#include <math.h>

#include "dis.h"

dis_addr_t DIS_INT_MAX(const dis_base_t base, const dis_digits_t digits);
dis_addr_t DIS_INT_END(const dis_base_t base, const dis_digits_t digits);

dis_int_t rotate(const dis_base_t, const dis_digits_t, const dis_int_t);
dis_int_t subtract_without_borrow(const dis_base_t, dis_digits_t,
		dis_int_t, dis_int_t);

dis_addr_t dis_addr_add(
		const dis_base_t base, const dis_digits_t digits,
		const dis_addr_t x, const dis_addr_t y);

#endif /* _DIS_MATH_H */
