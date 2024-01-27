#ifndef vec_test_h
#define vec_test_h
#include "../include/linAlg/types.h"
#include "../include/linAlg/vec2.h"
#include "../include/linAlg/ivec2.h"
#include "../include/linAlg/vec3.h"
#include "../include/linAlg/ivec3.h"
#include "../include/linAlg/vec4.h"
#include "../include/linAlg/ivec4.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>

static void test_vec2dot() {
    printf("\tVec2 Dot Test... ");
    vec2float a = {1, 2};
    vec2float b = {3, 4};
    float expected = 11;
    if (vec2dot(a, b) == expected) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}
static void test_ivec2dot() {
    printf("\tIVec2 Dot Test... ");
    vec2int a = {1, 2};
    vec2int b = {3, 4};
    float expected = 11;
    if (ivec2dot(a, b) == expected) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}

static void test_vec2mult() {
    printf("\tVec2 Mult Test... ");
    vec2float a = {1, 2};
    vec2float b = {3, 4};
    vec2float expected = {3.0f, 8.0f};
    vec2float actual = vec2mult(a, b);
    
    if (vec2float_eq(actual, expected) == 1) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}
static void test_ivec2mult() {
    printf("\tIVec2 Mult Test... ");
    vec2int a = {1, 2};
    vec2int b = {3, 4};
    vec2int expected = {3, 8};
    vec2int actual = ivec2mult(a, b);
    
    if (vec2int_eq(actual, expected) == 1) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}


static void test_vec2add() {
    printf("\tVec2 Add Test... ");
    vec2float a = {1, 2};
    vec2float b = {3, 4};
    vec2float expected = {4, 6};
    vec2float actual = vec2add(a, b);

    if (vec2float_eq(actual, expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }


}
static void test_ivec2add() {
    printf("\tIVec2 Add Test... ");
    vec2int a = {1, 2};
    vec2int b = {3, 4};
    vec2int expected = {4, 6};
    vec2int actual = ivec2add(a, b);

    if (vec2int_eq(actual, expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }


}

static void test_vec3dot() {
    printf("\tVec3 Dot Test... ");
    vec3float a = {1, 2, 3};
    vec3float b = {3, 4, 2};
    float expected = 17;
    if (vec3dot(a, b) == expected) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}
static void test_ivec3dot() {
    printf("\tIVec3 Dot Test... ");
    vec3int a = {1, 2, 3};
    vec3int b = {3, 4, 2};
    int expected = 17;
    if (ivec3dot(a, b) == expected) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}

static void test_vec3mult() {
    printf("\tVec3 Mult Test... ");
    vec3float a = {1, 2, 3};
    vec3float b = {3, 4, 2};
    vec3float expected = {3.0f, 8.0f, 6.0f};
    vec3float actual = vec3mult(a, b);
    
    if (vec3float_eq(actual, expected) == 1) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}
static void test_ivec3mult() {
    printf("\tIVec3 Mult Test... ");
    vec3int a = {1, 2, 3};
    vec3int b = {3, 4, 2};
    vec3int expected = {3, 8, 6};
    vec3int actual = ivec3mult(a, b);
    
    if (vec3int_eq(actual, expected) == 1) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}


static void test_vec3add() {
    printf("\tVec3 Add Test... ");
    vec3float a = {1, 2, 3};
    vec3float b = {3, 4, 2};
    vec3float expected = {4, 6, 5};
    vec3float actual = vec3add(a, b);

    if (vec3float_eq(actual, expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}
static void test_ivec3add() {
    printf("\tIVec3 Add Test... ");
    vec3int a = {1, 2, 3};
    vec3int b = {3, 4, 2};
    vec3int expected = {4, 6, 5};
    vec3int actual = ivec3add(a, b);

    if (vec3int_eq(actual, expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}

static void test_vec3cross() {
    printf("\tVec3 Cross Test... ");
    vec3float a = {1, 2, 3};
    vec3float b = {3, 4, 2};
    vec3float expected = {-8, 7, -2};
    vec3float actual = vec3cross(a, b);

    if (vec3float_eq(actual, expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}
static void test_ivec3cross() {
    printf("\tIVec3 Cross Test... ");
    vec3int a = {1, 2, 3};
    vec3int b = {3, 4, 2};
    vec3int expected = {-8, 7, -2};
    vec3int actual = ivec3cross(a, b);

    if (vec3int_eq(actual, expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}




static void test_vec4dot() {
    
    printf("\tVec4 Dot Test... ");
    vec4float a = {1, 2, 3, 4};
    vec4float b = {3, 4, 2, 1};
    float expected = 21;
    if (vec4dot(a, b) == expected) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}
static void test_ivec4dot() {
    
    printf("\tIVec4 Dot Test... ");
    vec4int a = {1, 2, 3, 4};
    vec4int b = {3, 4, 2, 1};
    int expected = 21;
    if (ivec4dot(a, b) == expected) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}

static void test_vec4mult() {
    printf("\tVec4 Mult Test... ");
    vec4float a = {1, 2, 3, 4};
    vec4float b = {3, 4, 2, 1};
    vec4float expected = {3.0f, 8.0f, 6.0f, 4.0f};
    vec4float actual = vec4mult(a, b);
    
    if (vec4float_eq(actual, expected) == 1) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}
static void test_ivec4mult() {
    printf("\tIVec4 Mult Test... ");
    vec4int a = {1, 2, 3, 4};
    vec4int b = {3, 4, 2, 1};
    vec4int expected = {3, 8, 6, 4};
    vec4int actual = ivec4mult(a, b);
    
    if (vec4int_eq(actual, expected) == 1) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}


static void test_vec4add() {
    printf("\tVec4 Add Test... ");
    vec4float a = {1, 2, 3, 4};
    vec4float b = {3, 4, 2, 1};
    vec4float expected = {4, 6, 5, 5};
    vec4float actual = vec4add(a, b);

    if (vec4float_eq(actual, expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}
static void test_ivec4add() {
    printf("\tIVec4 Add Test... ");
    vec4int a = {1, 2, 3, 4};
    vec4int b = {3, 4, 2, 1};
    vec4int expected = {4, 6, 5, 5};
    vec4int actual = ivec4add(a, b);

    if (vec4int_eq(actual, expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}









static void test_vec() {
    test_vec2dot();
    test_vec2mult();
    test_vec2add();

    test_ivec2dot();
    test_ivec2mult();
    test_ivec2add();

    test_vec3dot();
    test_vec3mult();
    test_vec3add();
    test_vec3cross();

    test_ivec3dot();
    test_ivec3mult();
    test_ivec3add();
    test_ivec3cross();

    test_vec4dot();
    test_vec4mult();
    test_vec4add();
    
    test_ivec4dot();
    test_ivec4mult();
    test_ivec4add();
    
    //printf("\t\e[1;1H\e[2J");
    printf("Vec Tests... " GREEN "passed " CHECK NRM "\n");

};
#endif
