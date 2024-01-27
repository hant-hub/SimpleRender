#ifndef MATH_SQRT_H
#define MATH_SQRT_H
#include <stdint.h>


static const uint8_t iterations = 2;


static inline float sqrt(float x) {
    //Newton Iteration method
    //starts using bithack for initial guess
    union { float f; uint32_t i; } val = {x};
    val.i = (1<<29) + (val.i >> 1) - (1<<22);

    for (int i = 0; i<iterations; i++) {
        val.f = (val.f + x/val.f)/2;
    }

    return val.f;
}




#endif
