#ifndef MATH_IVEC2_H
#define MATH_IVEC2_H
#include "types.h"


def_vector(int, 2);

/** 
 *
 * @brief Determine Whether two vec2int are equivalent.
 *
 * @param{vec2int} a Two component int vector.
 * @param{vec2int} b Two component int vector.
 *
 * @return Returns a boolean which evaluates whether two vectors are equivalent.
 * */
static inline const int vec2int_eq(vec2int a, vec2int b) {
    return ((a.val[0] == b.val[0]) && (a.val[1] == b.val[1]));
}



/**
 * 
 * @brief Take the dot product of two vectors.
 * @param{vec2int} a Two component int vector.
 * @param{vec2int} b Two component int vector.
 *
 *
 * @return Returns the int result of dot product.
 *
 */
static inline const int ivec2dot(vec2int a, vec2int b) {
    return a.val[0] * b.val[0] + a.val[1] * b.val[1];
}

/**
 * @brief Do a Component-wise multiplication.
 * @param{vec2int} a Two component  int vector.
 * @param{vec2int} b Two component  int vector.
 *
 * @return Returns a vector containing the component wise multiplication.
 * */
static inline const vec2int ivec2mult(vec2int a, vec2int b) {
    return (vec2int){a.val[0]*b.val[0], a.val[1]*b.val[1]};
}

/** 
 * @brief Scales a Vector
 * @param a Two component int vector.
 * @param s scalar int.
 *
 * @return Returns a vector scaled by s
 * */
static inline const vec2int ivec2scale(vec2int a, int s) {
    return (vec2int){a.val[0]*s, a.val[1]*s};
}



/**
 *
 * @brief Add vectors component wise.
 *
 * @param a Two component  int vector.
 * @param b Two component  int vector.
 *
 * @return Returns a vector containing the component wise addition.
 * */
static inline const vec2int ivec2add(vec2int a, vec2int b) {
    return (vec2int){a.val[0]+b.val[0], a.val[1]+b.val[1]};
}


/**
 *
 * @brief Subtracts vectors component wise.
 *
 * @param a Two component  int vector.
 * @param b Two component  int vector.
 *
 * @return Returns a vector containing the component wise difference.
 * */
static inline const vec2int ivec2sub(vec2int a, vec2int b) {
    return (vec2int){a.val[0]-b.val[0], a.val[1]-b.val[1]};
}


#endif
