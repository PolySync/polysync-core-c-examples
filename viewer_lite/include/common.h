/*
 * Copyright (c) 2015 HARBRICK TECHNOLOGIES, INC
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file common.h
 * @brief Common Resources.
 *
 */

#ifndef COMMON_H
#define	COMMON_H




#include <math.h>
#include <glib-2.0/glib.h>

#include "polysync_core.h"




/**
 * @brief Convert radians to degrees. [double]
 *
 * angle = 180 / pi
 *
 */
#define         RAD_2_DEG(angle)        ((angle)*(180.0/M_PI))


/**
 * @brief Convert degrees to radians. [double]
 *
 * angle = pi / 180
 *
 */
#define         DEG_2_RAD(angle)        ((angle)*(M_PI/180.0))


/**
 * @brief Convert seconds to microseconds. [unsigned long long]
 *
 */
#define         SEC_2_MICRO(time)       (time * 1000000ULL)


/**
 * @brief Convert microseconds to seconds. [double]
 *
 */
#define         MICRO_2_SEC(time)       (time / 1000.0 / 1000.0)


/**
 * @brief Convert nanoseconds to microseconds. [unsigned long long]
 *
 */
#define         NANO_2_MICRO(time)      (time / 1000ULL)




/**
 * @brief Get monotonic tick. [microseconds]
 *
 * @return Microsecond tick value.
 *
 */
ps_timestamp get_micro_tick( void );




#endif	/* COMMON_H */
