#ifndef MATH_ACOS_H
#define MATH_ACOS_H
#include "types.h"
#include <stdio.h>

/* 
 * Currently only planning to support a single 
 * accuracy level and algorithm, but may add
 * variable accuracy and speed settings later.
 *
 * */
#define INVERSE_ITER 10


/* 
 * @brief Compute inverse Cos function in Radians
 *
 * @param x Value to be inverted 
 * @return Returns an angle in radians 
 *
 * */
static inline const radians acos_r(float x) {
    cfloat check = ipowerTable[0];    
    float out = ipowerref[0];

    int n = ((int)x>=0)*2 - 1;
    x *= n;

    for (int i = 1; i < iterationCount; i++) {
        if (check.r*pi/2 == x){
            break; 
        }
        if (check.r > x) {
            check = cf_mult(check, ipowerTable[i]);
            out += ipowerref[i];
        }
        if (check.r < x) {
            check = cf_divide(check, ipowerTable[i]);
            out -= ipowerref[i];
        }
    }
    out *= pi/2;

    if (n == -1) out = pi - out;
    return (radians){out};
}

static inline const degrees acos_d(float theta) {
    return RadianstoDegrees(acos_r(theta));
}



#endif
