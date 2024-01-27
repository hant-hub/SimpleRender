#ifndef MATH_MAT3_H
#define MATH_MAT3_H
#include "types.h"
#include "../error.h"
#include "vec3.h"

def_mat(float, 3, 3);

const static mat3x3_float mat3x3_float_identity = {{
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1}
}};


/** 
 *
 * @brief Tests if two mat3x3 are equivalent
 * @param a A 3x3 matrix with floating point entries
 * @param b A 3x3 matrix with floating point entries
 *
 * @return Boolean representing whether the two matricies are equivalent.
 * */
static inline const int mat3x3_float_eq(mat3x3_float* a, mat3x3_float* b) {
    return (a->val[0][0] == b->val[0][0]) &&
           (a->val[1][0] == b->val[1][0]) &&
           (a->val[2][0] == b->val[2][0]) &&
           (a->val[0][1] == b->val[0][1]) &&
           (a->val[1][1] == b->val[1][1]) &&
           (a->val[2][1] == b->val[2][1]) &&
           (a->val[0][2] == b->val[0][2]) &&
           (a->val[1][2] == b->val[1][2]) &&
           (a->val[2][2] == b->val[2][2]);
}

/** 
 *
 * @brief Sums two matricies
 * @param a A 3x3 matrix with floating point entries
 * @param b A 3x3 matrix with floating point entries
 *
 * @return 3x3 matrix with final matrix
 * */
static inline const mat3x3_float mat3x3add(mat3x3_float* a, mat3x3_float* b) {
    return (mat3x3_float){{ 
        {a->val[0][0] + b->val[0][0], a->val[0][1] + b->val[0][1], a->val[0][2] + b->val[0][2]},
        {a->val[1][0] + b->val[1][0], a->val[1][1] + b->val[1][1], a->val[1][2] + b->val[1][2]},
        {a->val[2][0] + b->val[2][0], a->val[2][1] + b->val[2][1], a->val[2][2] + b->val[2][2]}
    }};
}


/** 
 * @brief Calculates the Determinant of a Matrix
 *
 * @param a A 3x3 matrix with floating point entries
 * @return Returns a float with the determinant
 * */
static inline const float mat3x3det(mat3x3_float* m) {
    return (m->val[0][0] * m->val[1][1] * m->val[2][2] +
            m->val[0][1] * m->val[1][2] * m->val[2][0] +
            m->val[0][2] * m->val[1][0] * m->val[2][1]) -
           (m->val[2][0] * m->val[1][1] * m->val[0][2] +
            m->val[2][1] * m->val[1][2] * m->val[0][0] +
            m->val[2][2] * m->val[1][0] * m->val[0][1]);
}

/** 
 * @brief Calculates vector Matrix multiply
 * @param m Matrix to be Multiplied
 * @param v Vector to be Multiplied
 * @return Vector which has been transformed.
 * */
static inline const vec3float mat3x3app(mat3x3_float* m, vec3float* v) {
    return (vec3float){{
        v->val[0] * m->val[0][0] + v->val[1] * m->val[0][1] + v->val[2] * m->val[0][2],
        v->val[0] * m->val[1][0] + v->val[1] * m->val[1][1] + v->val[2] * m->val[1][2],
        v->val[0] * m->val[2][0] + v->val[1] * m->val[2][1] + v->val[2] * m->val[2][2],
    }};
}


/** 
 *
 * @brief Applies Matrix A to B
 * @param a A 3x3 matrix with floating point entries
 * @param b A 3x3 matrix with floating point entries
 *
 * @return Returns the composition of A and B
 *
 * */
static inline const mat3x3_float mat3x3comp(mat3x3_float* a, mat3x3_float* b) {
    vec3float col1 = (vec3float){b->val[0][0], b->val[1][0], b->val[2][0]};    
    vec3float col2 = (vec3float){b->val[0][1], b->val[1][1], b->val[2][1]};    
    vec3float col3 = (vec3float){b->val[0][2], b->val[1][2], b->val[2][2]};    

    vec3float t1 = mat3x3app(a, &col1); 
    vec3float t2 = mat3x3app(a, &col2); 
    vec3float t3 = mat3x3app(a, &col3); 

    return (mat3x3_float) {{
        {t1.val[0], t2.val[0], t3.val[0]},
        {t1.val[1], t2.val[1], t3.val[1]},
        {t1.val[2], t2.val[2], t3.val[2]},
    }};

}


// Elementary Row Operations

/** 
 * @brief Scales a matrix Row
 * @param m Matrix to be modified
 * @param s floating point scalar
 * @param row Index of row to be scaled
 *
 * */
static inline const void mat3x3RowMult(mat3x3_float* m, float s, int row) {
    m->val[row][0] *= s;  
    m->val[row][1] *= s;
    m->val[row][2] *= s;
}

/** 
 * @brief Swap two rows in a matrix
 * @param m Matrix to be modified
 * @param row1 First Index of matrix
 * @param row2 Second Index of matrix
 *
 * */
static inline const void mat3x3RowSwap(mat3x3_float* m, int row1, int row2) {
    float temp[3] = {m->val[row1][0], m->val[row1][1], m->val[row1][2]};

    m->val[row1][0] = m->val[row2][0];
    m->val[row1][1] = m->val[row2][1];
    m->val[row1][2] = m->val[row2][2];

    m->val[row2][0] = temp[0];
    m->val[row2][1] = temp[1];
    m->val[row2][2] = temp[2];
}

/** 
 * @brief Replace Row with sum of Itself and another row
 * @param m Matrix to be modified
 * @param src Row that won't be modified
 * @param dest Row to be modified
 * @param s Scalar which is multiplied with src row before addition
 * */
static inline const void mat3x3RowAdd(mat3x3_float* m, int src, int dest, float s) {
    m->val[dest][0] += m->val[src][0] * s;
    m->val[dest][1] += m->val[src][1] * s;
    m->val[dest][2] += m->val[src][2] * s;
}




#endif
