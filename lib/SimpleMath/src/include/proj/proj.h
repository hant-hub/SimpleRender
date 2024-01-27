#ifndef MATH_PROJ_H
#define MATH_PROJ_H 
#include "../linAlg/mat4x4.h"
#include "../complex/rotations.h"
#include "../trig/tan.h"
#include "../trig/sin.h"
#include "../trig/cos.h"
#include "../trig/types.h"
#include <error.h>


//utility functions

static inline const ErrorCode mat4_translate(mat4x4_float* m, vec3float t) {
    m->val[0][3] += t.val[0];
    m->val[1][3] += t.val[1];
    m->val[2][3] += t.val[2];

    return NoError;
}

static inline const ErrorCode mat4_scale(mat4x4_float* m, float s) {
    m->val[0][0] *= s;
    m->val[1][1] *= s;
    m->val[2][2] *= s;

    return NoError;
}

static inline const ErrorCode mat4_rotate(mat4x4_float* m, EulerAngles a) {
//Plan to implement rotors for efficient rotations in the future
    float sinx = sin_r(a.x);
    float cosx = cos_r(a.x);

    mat4x4_float rx = {{
        {1,  0,    0,    0},
        {0,  cosx, sinx, 0},
        {0, -sinx, cosx, 0},
        {0,  0,    0,    1}
    }};

    float siny = sin_r(a.y);
    float cosy = cos_r(a.y);
    
    mat4x4_float ry = {{
        {cosy, 0, -siny, 0},
        {0,    1,  0,    0},
        {siny, 0,  cosy, 0},
        {0,    0,  0,    1}
    }};

    float sinz = sin_r(a.z);
    float cosz = cos_r(a.z);

    mat4x4_float rz = {{
        {cosz, -sinz, 0, 0},
        {sinz,  cosz, 0, 0},
        {0,     0,    1, 0},
        {0,     0,    0, 1}
    }};

    mat4x4_float temp = mat4x4comp(&ry, &rz);
    *m = mat4x4comp(&rx, &temp);

    return NoError;
}




//rendering constructors


static inline const ErrorCode mat4_PerspectiveMatrix(mat4x4_float* m, float near, float far, radians fov, float aspect){ 
    float tanfov = tan_r((radians){fov.val/2.0f});
    float nearfardiff = far-near;

    m->val[0][0] = 1.0f/(aspect * tanfov); 
    m->val[1][1] = 1.0f/tanfov;
    m->val[2][2] = -(far+near)/nearfardiff;
    m->val[3][2] = -1;
    m->val[2][3] = -(2*far*near)/nearfardiff;

    return NoError;
}

static inline const ErrorCode mat4_OrthoMatrix(mat4x4_float* m, float left, float right, float top, float bottom, float near, float far) {

    float horizontaldiff = right - left;
    float verticaldiff   = top - bottom;
    float depthdiff      = far - near;

    m->val[0][0] = 2/horizontaldiff;
    m->val[1][1] = 2/verticaldiff;
    m->val[2][2] = -2/depthdiff;
    
    m->val[0][3] = -(right+left)/horizontaldiff;
    m->val[1][3] = -(top+bottom)/verticaldiff;
    m->val[2][3] = -(far+near)/depthdiff;

    m->val[3][3] = 1;

    return NoError;
}

static inline const ErrorCode mat4_ViewMatrix(mat4x4_float* m, vec3float pos, vec3float lookat, vec3float up) {
    mat4x4_float t = {0};

    mat4_translate(&t, vec3scale(pos, -1));    

    vec3float f = vec3norm(vec3sub(pos, lookat));
    vec3float l = vec3norm(vec3cross(up, f));
    vec3float u = vec3cross(f,l);
    
    mat4x4_float r = {{ 
        {l.val[0], l.val[1], l.val[2], 0},
        {u.val[0], u.val[1], u.val[2], 0},
        {f.val[0], f.val[2], f.val[2], 0},
        {0,        0,        0,        1}
    }};

    *m = mat4x4comp(&r, &t);

    return NoError;
}





#endif
