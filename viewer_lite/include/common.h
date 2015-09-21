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
