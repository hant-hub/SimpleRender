#ifndef MATH_TAN_H
#define MATH_TAN_H
#include "types.h"
#include "cos.h"
#include "sin.h"

/* 
 * @brief Calculates Tangent in Radians
 * @param theta Angle in Radians
 *
 * @return Returns a floating point value
 *
 * */
static inline const float tan_r(radians theta) {
    return sin_r(theta)/cos_r(theta);
}


/* 
 * @brief Calculates Tangent in Degrees
 * @param theta Angle in Degrees
 *
 * @return Returns a floating point value
 *
 * */
static inline const float tan_d(degrees theta) {
    radians angle = DegreestoRadians(theta);
    return sin_r(angle)/cos_r(angle);
}


#endif
