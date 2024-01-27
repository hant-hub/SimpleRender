#ifndef MATH_IVEC4_H
#define MATH_IVEC4_H
#include "types.h"


def_vector(int, 4);


/**
 * @brief Determines whether two vectors are identical
 *
 * @param a Four component int vector.
 * @param b Four component int vector.
 * 
 * @return Boolean value indicating equality.
 * */
static inline const int vec4int_eq(vec4int a, vec4int b) {
    return (a.val[0] == b.val[0] &&
           a.val[1] == b.val[1] &&
           a.val[2] == b.val[2] &&
           a.val[3] == b.val[3]);
}

/** 
 *
 * @brief Returns Component sum of two 4 component Vectors.
 *
 * @param a Four component int vector.
 * @param b Four component int vector.
 *
 * @return Four Component Sum.
 * */
static inline const vec4int ivec4add(vec4int a, vec4int b) {
    return (vec4int){
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
 * @param a Four component int vector.
 * @param b Four component int vector.
 *
 * @return Four Component Difference.
 * */
static inline const vec4int ivec4sub(vec4int a, vec4int b) {
    return (vec4int){
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
 * @param a Four component int vector.
 * @param b Four component int vector.
 *
 * @return Four Component Dot product
 *
 * */
static inline const int ivec4dot(vec4int a, vec4int b) {
    return ((a.val[0] * b.val[0]) +
            (a.val[1] * b.val[1]) +
            (a.val[2] * b.val[2]) +
            (a.val[3] * b.val[3]));
}




/** 
 *
 * @brief Returns Component Multiplication of two 4 component vectors
 *
 * @param a Four component int vector.
 * @param b Four component int vector.
 *
 * @return Four Component Vector multiply
 *
 * */
static inline const vec4int ivec4mult(vec4int a, vec4int b) {
    return (vec4int){
        a.val[0] * b.val[0],
        a.val[1] * b.val[1],
        a.val[2] * b.val[2],
        a.val[3] * b.val[3]
    };
}

/** 
 * @brief Scales a Vector
 * @param a four component int vector.
 * @param s scalar int.
 *
 * @return Returns a vector scaled by s
 * */
static inline const vec4int ivec4scale(vec4int a, int s) {
    return (vec4int){a.val[0]*s, a.val[1]*s, a.val[2]*s, a.val[3] *s};
}

#endif
