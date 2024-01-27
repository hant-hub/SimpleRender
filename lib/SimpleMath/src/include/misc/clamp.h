#ifndef MATH_CLAMP_H
#define MATH_CLAMP_H



static inline float clampf(float in, float min, float max) {
    const float t = in < min ? min : in;
    return t > max ? max : t;
}


static inline double clampd(double in, double min, double max) {
    const double t = in < min ? min : in;
    return t > max ? max : t;
}




#endif
