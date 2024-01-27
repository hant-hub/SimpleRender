#ifndef MATH_TRIG_TYPES_H
#define MATH_TRIG_TYPES_H
#include "../complex/cfloat.h"

typedef struct radians { float val; } radians;
typedef struct degrees { float val; } degrees;

const static float pi = 3.141592;

/*
 * Variable which controls the precision of Inverse Trig Functions
 * maximum = 14
 * minimum = 1
 * */
static unsigned int iterationCount = 10;


static inline const radians normalize_r(radians theta) {
    float x = theta.val;

    float sub = x/(2*pi);

    float whole = (long int)sub;
    float frac = sub - whole;

    return (radians){frac*2*pi};
}

static inline const degrees normalize_d(degrees theta) {
    return (degrees){0};
}


static inline const radians DegreestoRadians(degrees theta) {
    static const float ratio = 1.0f/360.0f;
    float x = theta.val;
    x *= ratio;
    x *= 2*pi;
    return (radians){x};
}

static inline const degrees RadianstoDegrees(radians theta) {
    static const float ratio = 1.0f/(2*pi);
    float x = theta.val;
    x *= ratio;
    x *= 360;
    return (degrees){x};
}


// Precomputed powers of i
static const cfloat ipowerTable[] = {
    {1, 0}, //0
    {0, 1}, //1
    {0.707106781, 0.707106781},    // 1/2
    {0.9238795325, 0.3826834324},  // 1/4
    {0.9807852804, 0.1950903220},  // 1/8
    {0.9951847267, 0.0980171403},  // 1/16
    {0.9987954562, 0.0490676743},  // 1/32
    {0.9996988187, 0.0245412285},  // 1/64
    {0.9999247018, 0.0122715383},  // 1/128
    {0.9999811753, 0.0061358846},  // 1/256
    {0.9999952938, 0.0030679568},  // 1/512
    {0.9999988235, 0.0015339802},  // 1/1024
    {0.9999997058628822, 0.0007669903187427}, // 1/2048 
    {0.9999999265, 0.0003834952}, // 1/4096
    {0.9999999816, 0.0001917476},  // 1/8192
    {0.9999999954, 0.0000958738},  // 1/16384
};

static const float ipowerref[] = {
    0,
    1,
    0.5,    // 1/2
    0.25,  // 1/4
    0.125,  // 1/8
    0.0625,  // 1/16
    0.03125,  // 1/32
    0.015625,  // 1/64
    0.0078125,  // 1/128
    0.00390625,  // 1/256
    0.001953125,  // 1/512
    0.0009765625,  // 1/1024
    0.00048828125,  // 1/2048
    0.000244140625,  // 1/4096
    0.0001220703125,  // 1/8192
    0.00006103515625   // 1/16384
};

#endif
