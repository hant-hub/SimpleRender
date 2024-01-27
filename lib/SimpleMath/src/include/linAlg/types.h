#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H
#include <stdlib.h>



#define EPS 0.0001

/** 
 *
 * @brief Macro to define vector types
 *
 * @param{type} type is a defined type to store.
 * @param{size} Size is how many elements there are to store.
 *
 * */
#define def_vector(type, size) \
    typedef struct vec##size##type { \
        type val[size]; \
    } vec##size##type


/** 
 * @brief Macro to define matrix types
 * @param type A type to store.
 * @param rows The number of rows in the matrix.
 * @param col The number of columns in the matrix.
 * */
#define def_mat(type, rows, col) \
    typedef struct mat##rows##x##col##_##type { \
        type val[rows][col]; \
    } mat##rows##x##col##_##type



#endif
