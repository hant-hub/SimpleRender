#ifndef MATH_MAT2_H
#define MATH_MAT2_H
#include "types.h"
#include "../error.h"
#include "vec2.h"

def_mat(float, 2, 2);



const static mat2x2_float mat2x2_float_identity = {{
    {1, 0},
    {0, 1}
}};






/** 
 *
 * @brief Tests if two mat2x2 are equivalent
 * @param a A 2x2 matrix with floating point entries
 * @param b A 2x2 matrix with floating point entries
 *
 * @return Boolean representing whether the two matricies are equivalent.
 * */
static inline const int mat2x2_float_eq(mat2x2_float* a, mat2x2_float* b) {
    return (a->val[0][0] == b->val[0][0]) &&
           (a->val[1][0] == b->val[1][0]) &&
           (a->val[0][1] == b->val[0][1]) &&
           (a->val[1][1] == b->val[1][1]);
}

/** 
 *
 * @brief Sums two matricies
 * @param a A 2x2 matrix with floating point entries
 * @param b A 2x2 matrix with floating point entries
 *
 * @return 2x2 matrix with final matrix
 * */
static inline const mat2x2_float mat2x2add(mat2x2_float* a, mat2x2_float* b) {
    return (mat2x2_float){{ 
        {a->val[0][0] + b->val[0][0], a->val[0][1] + b->val[0][1]},
        {a->val[1][0] + b->val[1][0], a->val[1][1] + b->val[1][1]}
    }};
}


/** 
 * @brief Calculates the Determinant of a Matrix
 *
 * @param a A 2x2 matrix with floating point entries
 * @return Returns a float with the determinant
 * */
static inline const float mat2x2det(mat2x2_float* m) {
    return (m->val[0][0] * m->val[1][1]) -
        (m->val[1][0] * m->val[0][1]);
}

/** 
 *
 * @brief Applies Matrix A to B
 * @param a A 2x2 matrix with floating point entries
 * @param b A 2x2 matrix with floating point entries
 *
 * @return Returns the composition of A and B
 *
 * */
static inline const mat2x2_float mat2x2comp(mat2x2_float* a, mat2x2_float* b) {
    return (mat2x2_float){{
        {a->val[0][0] * b->val[0][0] + a->val[0][1] * b->val[1][0],
        a->val[0][0] * b->val[0][1] + a->val[0][1] * b->val[1][1]},
        {a->val[1][0] * b->val[0][0] + a->val[1][1] * b->val[1][0],
        a->val[1][0] * b->val[0][1] + a->val[1][1] * b->val[1][1]}
    }};
}


/** 
 *
 * @brief Applies Matrix A to vector
 *
 * @param m A 2x2 Matrix
 * @param v A 2 component Vector
 *
 * @return A 2 component Vector with A applied.
 * */
static inline const vec2float mat2x2app(mat2x2_float* m, vec2float* v) {
    return (vec2float){
    {
    (v->val[0] * m->val[0][0]) + (v->val[1] * m->val[0][1]),
    (v->val[0] * m->val[1][0]) + (v->val[1] * m->val[1][1])
    }};
}


// Elementary Row Operations

/** 
 * @brief Scales a matrix Row
 * @param m 2x2 matrix to be modified
 * @param s floating point scalar
 * @param row Index of row to be scaled
 *
 * */
static inline const void mat2x2RowMult(mat2x2_float* m, float s, int row) {
    m->val[row][0] = m->val[row][0]*s;  
    m->val[row][1] = m->val[row][1]*s;
}

/** 
 * @brief Swap two rows in a matrix
 * @param m 2x2 matrix to be modified
 * @param row1 First Index of matrix
 * @param row2 Second Index of matrix
 *
 * */
static inline const void mat2x2RowSwap(mat2x2_float* m, int row1, int row2) {
    float temp[2] = {m->val[row1][0], m->val[row1][1]};

    m->val[row1][0] = m->val[row2][0];
    m->val[row1][1] = m->val[row2][1];

    m->val[row2][0] = temp[0];
    m->val[row2][1] = temp[1];
}

/** 
 * @brief Replace Row with sum of Itself and another row
 * @param m Matrix to be modified
 * @param src Row that won't be modified
 * @param dest Row to be modified
 * @param s Scalar which is multiplied with src row before addition
 * */
static inline const void mat2x2RowAdd(mat2x2_float* m, int src, int dest, float s) {
    m->val[dest][0] += m->val[src][0] * s;
    m->val[dest][1] += m->val[src][1] * s;
}




#endif
