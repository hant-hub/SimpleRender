#ifndef MATH_VEC2_H
#define MATH_VEC2_H
#include "types.h"
#include "../misc/sqrt.h"

def_vector(float, 2);





/** 
 *
 * @brief Determine Whether two vec2float are equivalent.
 *
 * @param{vec2float} a Two component Floating point vector.
 * @param{vec2float} b Two component Floating point vector.
 *
 * @return Returns a boolean which evaluates whether two vectors are equivalent.
 * */
static inline int vec2float_eq(vec2float a, vec2float b) {
    return ((a.val[0] == b.val[0]) && (a.val[1] == b.val[1]));
}



/**
 * 
 * @brief Take the dot product of two vectors.
 * @param{vec2float} a Two component Floating point vector.
 * @param{vec2float} b Two component Floating point vector.
 *
 *
 * @return Returns the float result of dot product.
 *
 */
static inline const float vec2dot(vec2float a, vec2float b) {
    return a.val[0] * b.val[0] + a.val[1] * b.val[1];
}

/**
 * @brief Do a Component-wise multiplication.
 * @param{vec2float} a Two component Floating point vector.
 * @param{vec2float} b Two component Floating point vector.
 *
 * @return Returns a vector containing the component wise multiplication.
 * */
static inline const vec2float vec2mult(vec2float a, vec2float b) {
    return (vec2float){a.val[0]*b.val[0], a.val[1]*b.val[1]};
}

/** 
 * @brief Scales a Vector
 * @param a Two component float vector.
 * @param s scalar float.
 *
 * @return Returns a vector scaled by s
 * */
static inline const vec2float vec2scale(vec2float a, float s) {
    return (vec2float){a.val[0]*s, a.val[1]*s};
}



/**
 *
 * @brief Add vectors component wise.
 *
 * @param a Two component Floating point vector.
 * @param b Two component Floating point vector.
 *
 * @return Returns a vector containing the component wise addition.
 * */
static inline const vec2float vec2add(vec2float a, vec2float b) {
    return (vec2float){a.val[0]+b.val[0], a.val[1]+b.val[1]};
}

/**
 *
 * @brief Subtracts vectors component wise.
 *
 * @param a Two component Floating point vector.
 * @param b Two component Floating point vector.
 *
 * @return Returns a vector containing the component wise difference.
 * */
static inline const vec2float vec2sub(vec2float a, vec2float b) {
    return (vec2float){a.val[0]-b.val[0], a.val[1]-b.val[1]};
}


static inline const vec2float vec2norm(vec2float v) {
    float len = v.val[0]*v.val[0] + v.val[1]*v.val[1];
    len = sqrt(len);
    return vec2scale(v, 1/len);
}



#endif
