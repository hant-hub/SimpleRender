#ifndef MATH_CFLOAT_H
#define MATH_CFLOAT_H


/* 
 * @brief A complex number represented by 2 floats
 *
 * @member r The real part
 * @member i The imaginary part
 * */
typedef struct cfloat {
    float r;
    float i;
} cfloat;


static inline cfloat cf_add(cfloat a, cfloat b) {
    return (cfloat){a.r + b.r, a.i + b.i};
}

static inline cfloat cf_sub(cfloat a, cfloat b) {
    return (cfloat){a.r - b.r, a.i - b.i};
}

static inline cfloat cf_mult(cfloat a, cfloat b) {
    return (cfloat){
        a.r * b.r - a.i * b.i,
        a.r * b.i + b.r * a.i};
}

static inline cfloat cf_divide(cfloat a, cfloat b) {
    float denominator = (b.r * b.r) + (b.i * b.i);
    return (cfloat){
        (a.r * b.r + a.i * b.i)/denominator,
        (a.i * b.r - a.r * b.i)/denominator};
}

static inline cfloat cf_int_exp(cfloat a, int b) {
    for (int i = 0; i<b; i++) {
        a = cf_mult(a, a);
    }
    return a;
}





#endif
