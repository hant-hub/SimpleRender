#ifndef MATH_IVEC3_H
#define MATH_IVEC3_H
#include "types.h"

def_vector(int, 3);


/** 
 *
 * @brief Determine Whether two vec3int are equivalent.
 *
 * @param{vec3int} a Three component int vector.
 * @param{vec3int} b Three component int vector.
 *
 * @return Returns a boolean which evaluates whether two vectors are equivalent.
 * */
static inline const int vec3int_eq(vec3int a, vec3int b) {
    return ((a.val[0] == b.val[0]) && 
            (a.val[1] == b.val[1]) &&
            (a.val[2] == b.val[2]));
}



/**
 * 
 * @brief Take the dot product of three vectors.
 * @param{vec3int} a Three component int vector.
 * @param{vec3int} b Three component int vector.
 *
 *
 * @return Returns the int result of dot product.
 *
 */
static inline const int ivec3dot(vec3int a, vec3int b) {
    return a.val[0] * b.val[0] + 
           a.val[1] * b.val[1] +
           a.val[2] * b.val[2];
}

/**
 * @brief Do a Component-wise multiplication.
 * @param{vec3int} a Three component int vector.
 * @param{vec3int} b Three component int vector.
 *
 * @return Returns a vector containing the component wise multiplication.
 * */
static inline const vec3int ivec3mult(vec3int a, vec3int b) {
    return (vec3int){a.val[0]*b.val[0], 
                       a.val[1]*b.val[1],
                       a.val[2]*b.val[2]};
}


/** 
 * @brief Scales a Vector
 * @param a three component int vector.
 * @param s scalar int.
 *
 * @return Returns a vector scaled by s
 * */
static inline const vec3int ivec3scale(vec3int a, int s) {
    return (vec3int){a.val[0]*s, a.val[1]*s, a.val[2]*s};
}



/**
 *
 * @brief Add vectors component wise.
 *
 * @param{vec3int} a Three component int vector.
 * @param{vec3int} b Three component int vector.
 *
 * @return Returns a vector containing the component wise addition.
 * */
static inline const vec3int ivec3add(vec3int a, vec3int b) {
    return (vec3int){a.val[0]+b.val[0],
                       a.val[1]+b.val[1],
                       a.val[2]+b.val[2]};
}

/**
 *
 * @brief Subtracts vectors component wise.
 *
 * @param{vec3int} a Three component int vector.
 * @param{vec3int} b Three component int vector.
 *
 * @return Returns a vector containing the component wise subtraction.
 * */
static inline const vec3int ivec3sub(vec3int a, vec3int b) {
    return (vec3int){a.val[0]-b.val[0],
                       a.val[1]-b.val[1],
                       a.val[2]-b.val[2]};
}

/**
 *
 * @brief Right Handed Cross Product
 *
 * @param{vec3int} a Three component int vector.
 * @param{vec3int} b Three component int vector.
 *
 * @return Returns a vector which is the right handed cross product of a and b.
 *
 *
 * */
static inline const vec3int ivec3cross(vec3int a, vec3int b) {
    int first = (a.val[1] * b.val[2]) - (a.val[2] * b.val[1]);
    int second = (a.val[0] * b.val[2]) - (a.val[2] * b.val[0]);
    int third = (a.val[0] * b.val[1]) - (a.val[1] * b.val[0]);
    
    return (vec3int){first, -second, third};
}


#endif
