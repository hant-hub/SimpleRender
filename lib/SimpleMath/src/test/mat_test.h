#ifndef mat_test_h
#define mat_test_h
#include "../include/linAlg/types.h"
#include "../include/linAlg/mat2x2.h"
#include "../include/linAlg/mat3x3.h"
#include "../include/linAlg/mat4x4.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

static void test_mat2add() {
    printf("\tMat2x2 Add test... ");
    mat2x2_float a = {{
        {1, 1},
        {3, 4}
    }};

    mat2x2_float b = {{
        {5, 9},
        {3, 1}
    }};

    mat2x2_float expected = {{
        {6, 10},
        {6, 5}
    }};

    mat2x2_float actual = mat2x2add(&a, &b);
    if (mat2x2_float_eq(&expected, &actual)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
};


static void test_mat2det() {
    printf("\tMat2x2 Det test... ");
    mat2x2_float m = {{
        {1, 2},
        {3, 4}
    }};
    float expected = -2;

    if (expected == mat2x2det(&m)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}


static void test_mat2comp() {
    printf("\tMat2x2 Comp test... ");
    mat2x2_float a = {{
        {1, 1},
        {3, 4}
    }};

    mat2x2_float b = {{
        {5, 9},
        {3, 1}
    }};

    mat2x2_float expected = {{
        {8, 10},
        {27, 31}
    }};

    mat2x2_float actual = mat2x2comp(&a, &b);
    if (mat2x2_float_eq(&expected, &actual)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}


static void test_mat2app() {
    printf("\tMat2x2 App test... ");
    mat2x2_float m = {{
        {1, 2},
        {3, 4}
    }};
    vec2float v = {{1, 2}};

    vec2float expected = {{5, 11}};
    vec2float actual = mat2x2app(&m, &v); 


    if (vec2float_eq(expected, actual)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}


static void test_mat3add() {
    printf("\tMat3x3 Add test... ");
    mat3x3_float a = {{
        {1, 1, 2},
        {3, 4, 4},
        {3, 1, 4}
    }};

    mat3x3_float b = {{
        {5, 9, 4},
        {3, 1, 1},
        {2, 3, 5}
    }};

    mat3x3_float expected = {{
        {6, 10, 6},
        {6, 5, 5},
        {5, 4, 9}
    }};

    mat3x3_float actual = mat3x3add(&a, &b);
    if (mat3x3_float_eq(&expected, &actual)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}


static void test_mat3det() {
    printf("\tMat3x3 Det test... ");
    mat3x3_float m = {{
        {1, 1, 2},
        {3, 4, 4},
        {3, 1, 4}
    }};
    float expected = -6;

    if (expected == mat3x3det(&m)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}



static void test_mat3comp() {
    printf("\tMat3x3 Comp test... ");
    mat3x3_float a = {{
        {1, 1, 2},
        {3, 4, 4},
        {3, 1, 4}
    }};

    mat3x3_float b = {{
        {5, 9, 4},
        {3, 1, 1},
        {2, 3, 5}
    }};

    mat3x3_float expected = {{
        {12, 16, 15},
        {35, 43, 36},
        {26, 40, 33}
    }};

    mat3x3_float actual = mat3x3comp(&a, &b);
    if (mat3x3_float_eq(&expected, &actual)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}


static void test_mat3app() {
    printf("\tMat3x3 App test... ");
    mat3x3_float m = {{
        {1, 1, 2},
        {3, 4, 4},
        {3, 1, 4}
    }};
    vec3float v = {{1, 2, 3}};

    vec3float expected = {{9, 23, 17}};
    vec3float actual = mat3x3app(&m, &v); 


    if (vec3float_eq(expected, actual)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}



static void test_mat4add() {
    printf("\tMat4x4 Add test... ");
    mat4x4_float a = {{
        {1, 1, 2, 3},
        {3, 4, 4, -1},
        {3, 1, 4, 4},
        {5, 2, -1, 3}
    }};

    mat4x4_float b = {{
        {5, 9, 4, 1},
        {3, 1, 1, 9},
        {2, 3, 5, 10},
        {-2, 1.5, 4, 2}
    }};

    mat4x4_float expected = {{
        {6, 10, 6, 4},
        {6, 5, 5, 8},
        {5, 4, 9, 14},
        {3, 3.5, 3, 5}
    }};

    mat4x4_float actual = mat4x4add(&a, &b);
    if (mat4x4_float_eq(&expected, &actual)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}


static void test_mat4det() {
    printf("\tMat4x4 Det test... ");
    mat4x4_float m = {{
        {1, 1, 2, 3},
        {3, 4, 4, -1},
        {3, 1, 4, 4},
        {5, 2, -1, 3}
    }};
    float expected = -173;

    if (expected == mat4x4det(&m)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }

}



static void test_mat4comp() {
    printf("\tMat4x4 Comp test... ");
    mat4x4_float a = {{
        {1, 1, 2, 3},
        {3, 4, 4, -1},
        {3, 1, 4, 4},
        {5, 2, -1, 3}
    }};

    mat4x4_float b = {{
        {5, 9, 4, 1},
        {3, 1, 1, 9},
        {2, 3, 5, 10},
        {-2, 1.5, 4, 2}
    }};

    mat4x4_float expected = {{
        {6, 20.5, 27, 36},
        {37, 41.5, 32, 77},
        {18, 46, 49, 60},
        {23, 48.5, 29, 19}
    }};

    mat4x4_float actual = mat4x4comp(&a, &b);
    if (mat4x4_float_eq(&expected, &actual)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}


static void test_mat4app() {
    printf("\tMat4x4 App test... ");
    mat4x4_float m = {{
        {1, 1, 2, 3},
        {3, 4, 4, -1},
        {3, 1, 4, 4},
        {5, 2, -1, 3}
    }};
    vec4float v = {{1, 2, 3, 4}};

    vec4float expected = {{21, 19, 33, 18}};
    vec4float actual = mat4x4app(&m, &v); 


    if (vec4float_eq(expected, actual)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        exit(0);
    }
}

//write ERO tests
static void test_mat2ERO() {
    printf("\tMat2x2 ERO Testing... \n");
    int success = 1;


    printf("\t\t\tSwap Tests... ");
    mat2x2_float m = mat2x2_float_identity;
    mat2x2RowSwap(&m, 1, 0);

    mat2x2_float expected = (mat2x2_float){{
        {0, 1},
        {1, 0}
    }};


    if (mat2x2_float_eq(&m, &expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        success = 0;
    }
    
    printf("\t\t\tMult Tests... ");
    m = mat2x2_float_identity;
    mat2x2RowMult(&m, 2, 0);

    expected = (mat2x2_float){{
        {2, 0},
        {0, 1}
    }};


    if (mat2x2_float_eq(&m, &expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        success = 0;
    }


    printf("\t\t\tAdd Tests... ");
    m = mat2x2_float_identity;
    mat2x2RowAdd(&m, 1, 0, 2);

    expected = (mat2x2_float){{
        {1, 2},
        {0, 1}
    }};


    if (mat2x2_float_eq(&m, &expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        success = 0;
    }

    if (success) {
        printf("\t\tMat2x2 ERO " GREEN "passed" CHECK NRM "\n");
    } else {
        printf("\t\tMat2x2 ERO " RED "failed" CROSS NRM "\n");
        exit(0);
    }

}

static void test_mat3ERO() {
    printf("\tMat3x3 ERO Testing... \n");
    int success = 1;


    printf("\t\t\tSwap Tests... ");
    mat3x3_float m = mat3x3_float_identity;
    mat3x3RowSwap(&m, 1, 0);

    mat3x3_float expected = (mat3x3_float){{
        {0, 1, 0},
        {1, 0, 0},
        {0, 0, 1}
    }};


    if (mat3x3_float_eq(&m, &expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        success = 0;
    }
    
    printf("\t\t\tMult Tests... ");
    m = mat3x3_float_identity;
    mat3x3RowMult(&m, 2, 0);

    expected = (mat3x3_float){{
        {2, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    }};


    if (mat3x3_float_eq(&m, &expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        success = 0;
    }


    printf("\t\t\tAdd Tests... ");
    m = mat3x3_float_identity;
    mat3x3RowAdd(&m, 1, 0, 2);

    expected = (mat3x3_float){{
        {1, 2, 0},
        {0, 1, 0},
        {0, 0, 1}
    }};


    if (mat3x3_float_eq(&m, &expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        success = 0;
    }

    if (success) {
        printf("\t\tMat3x3 ERO " GREEN "passed" CHECK NRM "\n");
    } else {
        printf("\t\tMat3x3 ERO " RED "failed" CROSS NRM "\n");
        exit(0);
    }

}


static void test_mat4ERO() {
    printf("\tMat4x4 ERO Testing... \n");
    int success = 1;


    printf("\t\t\tSwap Tests... ");
    mat4x4_float m = mat4x4_float_identity;
    mat4x4RowSwap(&m, 1, 0);

    mat4x4_float expected = (mat4x4_float){{
        {0, 1, 0, 0},
        {1, 0, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};


    if (mat4x4_float_eq(&m, &expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        success = 0;
    }
    
    printf("\t\t\tMult Tests... ");
    m = mat4x4_float_identity;
    mat4x4RowMult(&m, 2, 0);

    expected = (mat4x4_float){{
        {2, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};


    if (mat4x4_float_eq(&m, &expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        success = 0;
    }


    printf("\t\t\tAdd Tests... ");
    m = mat4x4_float_identity;
    mat4x4RowAdd(&m, 1, 0, 2);

    expected = (mat4x4_float){{
        {1, 2, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};


    if (mat4x4_float_eq(&m, &expected)) {
        printf(GREEN "passed " CHECK NRM "\n");
    } else {
        printf(RED "failed " CROSS NRM "\n");
        success = 0;
    }

    if (success) {
        printf("\t\tMat4x4 ERO " GREEN "passed" CHECK NRM "\n");
    } else {
        printf("\t\tMat4x4 ERO " RED "failed" CROSS NRM "\n");
        exit(0);
    }

}
static void test_mat() {
    test_mat2add();
    test_mat2det();
    test_mat2comp();
    test_mat2app();

    test_mat3add();
    test_mat3det();
    test_mat3comp();
    test_mat3app();

    test_mat4add();
    test_mat4det();
    test_mat4comp();
    test_mat4app();

    test_mat2ERO();
    test_mat3ERO();
    test_mat4ERO();

    //printf("\t\e[1;1H\e[2J");
    printf("Mat Tests... " GREEN "passed " CHECK NRM "\n");
}


#endif
