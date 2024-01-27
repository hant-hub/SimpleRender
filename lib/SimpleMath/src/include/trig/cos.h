#ifndef MATH_COS_H
#define MATH_COS_H
#include "types.h"
#include <stdio.h>

/* 
 * Currently only planning to support a single 
 * accuracy level and algorithm, but may add
 * variable accuracy and speed settings later.
 *
 * */



/* 
 * @brief Compute Cos function in Radians
 *
 * Computes Cos using the Bhaskaraa approximation
 *
 *
 *
 *
 * @param theta Angle in radians
 * @return Returns a floating point value
 *
 * */
static inline const float cos_r(radians theta) {
    float x = normalize_r(theta).val;
    const float sqpi = pi*pi;
    float sign = 1;

    if (x < 0) {
        x *= -1;
    }

    if (x > (pi/2)) {
        x = (pi)-x;
        sign = -1;
    }

    float squared = x*x;
    float top = sign * (sqpi - 4*(squared));
    float bottom = sqpi + squared;
    float result = top/bottom;
    return result;
}

/* 
 * @brief Compute Cos function in Degrees
 *
 * Computes Cos using the Bhaskaraa approximation
 *
 *
 *
 *
 * @param theta Angle in degrees
 * @return Returns a floating point value
 *
 * */
static inline const float cos_d(degrees theta) {
    return cos_r(DegreestoRadians(theta));
}



#endif
