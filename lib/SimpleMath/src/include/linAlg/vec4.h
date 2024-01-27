#ifndef MATH_VEC4_H
#define MATH_VEC4_H
#include "types.h"
#include "../misc/sqrt.h"


def_vector(float, 4);


/**
 * @brief Determines whether two vectors are identical
 *
 * @param a Four component floating point vector.
 * @param b Four component floating point vector.
 * 
 * @return Boolean value indicating equality.
 * */
static inline int vec4float_eq(vec4float a, vec4float b) {
    return (a.val[0] == b.val[0] &&
           a.val[1] == b.val[1] &&
           a.val[2] == b.val[2] &&
           a.val[3] == b.val[3]);
}

/** 
 *
 * @brief Returns Component sum of two 4 component Vectors.
 *
 * @param a Four component floating point vector.
 * @param b Four component floating point vector.
 *
 * @return Four Component Sum.
 * */
static inline vec4float vec4add(vec4float a, vec4float b) {
    return (vec4float){
    a.val[0] + b.val[0],
    a.val[1] + b.val[1],
    a.val[2] + b.val[2],
    a.val[3] + b.val[3]
    };
}

/** 
 *
 * @brief Returns Component difference of two 4 component Vectors.
 *
 * @param a Four component floating point vector.
 * @param b Four component floating point vector.
 *
 * @return Four Component Difference.
 * */
static inline vec4float vec4sub(vec4float a, vec4float b) {
    return (vec4float){
    a.val[0] - b.val[0],
    a.val[1] - b.val[1],
    a.val[2] - b.val[2],
    a.val[3] - b.val[3]
    };
}


/** 
 *
 * @brief Returns Dot product of two 4 component vectors.
 *
 * @param a Four component floating point vector.
 * @param b Four component floating point vector.
 *
 * @return Four Component Dot product
 *
 * */
static inline float vec4dot(vec4float a, vec4float b) {
    return ((a.val[0] * b.val[0]) +
            (a.val[1] * b.val[1]) +
            (a.val[2] * b.val[2]) +
            (a.val[3] * b.val[3]));
}




/** 
 *
 * @brief Returns Component Multiplication of two 4 component vectors
 *
 * @param a Four component floating point vector.
 * @param b Four component floating point vector.
 *
 * @return Four Component Vector multiply
 *
 * */
static inline vec4float vec4mult(vec4float a, vec4float b) {
    return (vec4float){
        a.val[0] * b.val[0],
        a.val[1] * b.val[1],
        a.val[2] * b.val[2],
        a.val[3] * b.val[3]
    };
}


/** 
 * @brief Scales a Vector
 * @param a four component float vector.
 * @param s scalar float.
 *
 * @return Returns a vector scaled by s
 * */
static inline const vec4float vec4scale(vec4float a, float s) {
    return (vec4float){a.val[0]*s, a.val[1]*s, a.val[2]*s, a.val[3] * s};
}

static inline const vec4float vec4norm(vec4float v) {
    float len = v.val[0]*v.val[0] +
                v.val[1]*v.val[1] +
                v.val[2]*v.val[2] +
                v.val[3]*v.val[3];
    len = sqrt(len);
    return vec4scale(v, 1/len);
}

#endif
