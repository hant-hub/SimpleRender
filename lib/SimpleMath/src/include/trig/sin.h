#ifndef MATH_SIN_H
#define MATH_SIN_H
#include "types.h"
#include "cos.h"

/* 
 * Currently only planning to support a single 
 * accuracy level and algorithm, but may add
 * variable accuracy and speed settings later.
 *
 * */



/* 
 * @brief Compute Sin function in Radians
 *
 * Computes Sin using cos 
 *
 *
 *
 *
 * @param theta Angle in radians
 * @return Returns a floating point value
 *
 * */
static inline const float sin_r(radians theta) {
    theta.val -= pi/2;
    return cos_r(theta);
}

/* 
 * @brief Compute Sin function in Degrees
 *
 * Computes Sin using the Bhaskaraa approximation
 *
 *
 *
 *
 * @param theta Angle in degrees
 * @return Returns a floating point value
 *
 * */
static inline const float sin_d(degrees theta) {
    return sin_r(DegreestoRadians(theta));
}



#endif
