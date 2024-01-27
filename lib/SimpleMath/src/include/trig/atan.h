#ifndef MATH_ATAN_H
#define MATH_ATAN_H
#include "types.h"
#include <stdlib.h>


static inline const radians atan_r(float x) {
    cfloat check = ipowerTable[0];    
    float out = ipowerref[0];

    int n = ((int)x>=0)*2 - 1;
    x *= n;

    for (int i = 1; i < iterationCount; i++) {
        if ((check.i/check.r) == x){
            break; 
        }
        if ((check.i/check.r) < x) {
            check = cf_mult(check, ipowerTable[i]);
            out += ipowerref[i];
        }
        if ((check.i/check.r) > x) {
            check = cf_divide(check, ipowerTable[i]);
            out -= ipowerref[i];
        }
    }
    out *= pi/2;

    if (n == -1) out = -out;
    return (radians){out};
}

static inline const degrees atan_d(float x) {
    return RadianstoDegrees(atan_r(x));
}

static inline const radians atan2_r(float y, float x) {

    if (x == 0 && y > 0) {
        return (radians){pi/2};
    }

    if (x == 0 && y < 0) {
        return (radians){-pi/2};
    }

    if (x == 0 && y == 0) {
        exit(0);
    }


    float ratio = y/x;
    cfloat check = ipowerTable[0];    
    float out = ipowerref[0];

    int n = ((int)ratio>=0)*2 - 1;
    ratio *= n;

    for (int i = 1; i < iterationCount; i++) {
        if ((check.i/check.r) == ratio){
            break; 
        }
        if ((check.i/check.r) < ratio) {
            check = cf_mult(check, ipowerTable[i]);
            out += ipowerref[i];
        }
        if ((check.i/check.r) > ratio) {
            check = cf_divide(check, ipowerTable[i]);
            out -= ipowerref[i];
        }
    }
    out *= pi/2;
    if (n == -1) out = -out;



    if (x<0) {
        if (y >= 0) return (radians){out + pi}; 
        if (y < 0)  return (radians){out - pi};
    }


    return (radians){out};
}

static inline const degrees atan2_d(float y, float x) {
    return RadianstoDegrees(atan2_r(y,x));
}

#endif
