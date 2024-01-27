#ifndef TRIG_TEST_H
#define TRIG_TEST_H
#include "logger.h"
#include "../include/trig.h"
#include <stdio.h>
#include <stdlib.h>

static void cos_radians_test(float eps) {
    printf("\t\tCos: \n");
    radians theta = {0};
    float result;
    int success = 1;


    result = 1 - cos_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = pi;
    result = 1 + cos_r(theta);
    //printf("\t\t result = %f\n", result);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = pi/2;
    result = cos_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = -pi/2;
    result = cos_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 3*pi;
    result = 1 + cos_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 1;
    result =  0.540302305868 - cos_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 2;
    result = 0.416146836547 + cos_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 1.369;
    result = 0.200429529171 - cos_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    if (success) {
        printf(GREEN "\t\tpassed " CHECK NRM "\n");
    } else {
        printf(RED "\t\tfailed " CROSS NRM "\n");
        exit(0);
    }
}

static void sin_radians_test(float eps) {
    printf("\n\t\tSin: \n");
    radians theta = {0};
    float result;
    int success = 1;


    result = sin_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = pi;
    result = sin_r(theta);
    //printf("\t\t result = %f\n", result);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = pi/2;
    result = 1 - sin_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = -pi/2;
    result = 1 + sin_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 3*pi;
    result = sin_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 1;
    result =  0.8414709848 - sin_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 2;
    result = 0.9092974268 - sin_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 2.94;
    result = 0.2002299847 - sin_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    if (success) {
        printf(GREEN "\t\tpassed " CHECK NRM "\n");
    } else {
        printf(RED "\t\tfailed " CROSS NRM "\n");
        exit(0);
    }
}

static void tan_radians_test(float eps) {
    printf("\t\tTan: \n");
    radians theta = {0};
    float result;
    int success = 1;

    result = tan_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = pi/4;
    result = 1 - tan_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = -pi/4;
    result = 1 + tan_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = 1;
    result = 1.557407725 - tan_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = 2;
    result =  2.185039863 + tan_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = 3;
    result =  0.1425465431 + tan_r(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    if (success) {
        printf(GREEN "\t\tpassed " CHECK NRM "\n");
    } else {
        printf(RED "\t\tfailed " CROSS NRM "\n");
        exit(0);
    }
}

static void cos_degrees_test(float eps) {
    printf("\t\tCos: \n");
    degrees theta = {0};
    float result;
    int success = 1;


    result = 1 - cos_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 180;
    result = 1 + cos_d(theta);
    //printf("\t\t result = %f\n", result);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = 90;
    result = cos_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = -90;
    result = cos_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 540;
    result = 1 + cos_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 57.29577951;
    result =  0.540302305868 - cos_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 114.591559;
    result = 0.416146836547 + cos_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 78.43792215;
    result = 0.200429529171 - cos_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    if (success) {
        printf(GREEN "\t\tpassed " CHECK NRM "\n");
    } else {
        printf(RED "\t\tfailed " CROSS NRM "\n");
        exit(0);
    }
}


static void sin_degrees_test(float eps) {
    printf("\n\t\tSin: \n");
    degrees theta = {0};
    float result;
    int success = 1;


    result = sin_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 180;
    result = sin_d(theta);
    //printf("\t\t result = %f\n", result);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = 90;
    result = 1 - sin_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = -90;
    result = 1 + sin_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 540;
    result = sin_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 57.29577951;
    result =  0.8414709848 - sin_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 114.591559;
    result = 0.9092974268 - sin_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 168.4495918;
    result = 0.2002299847 - sin_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    if (success) {
        printf(GREEN "\t\tpassed " CHECK NRM "\n");
    } else {
        printf(RED "\t\tfailed " CROSS NRM "\n");
        exit(0);
    }
}


static void tan_degrees_test(float eps) {
    printf("\t\tTan: \n");
    degrees theta = {0};
    float result;
    int success = 1;

    result = tan_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = 45;
    result = 1 - tan_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }

    theta.val = -45;
    result = 1 + tan_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = 57.29577951;
    result = 1.557407725 - tan_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = 114.591559;
    result =  2.185039863 + tan_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    theta.val = 171.8873385;
    result =  0.1425465431 + tan_d(theta);
    if (result <= eps && result >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta.val);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta.val);
        printf("\t\t\t\tResult = %f\n", result);
        success = 0;
    }


    if (success) {
        printf(GREEN "\t\tpassed " CHECK NRM "\n");
    } else {
        printf(RED "\t\tfailed " CROSS NRM "\n");
        exit(0);
    }
}


static void acos_radians_test(float eps) {
    printf("\n\t\tInverse cos: \n");
    float theta = 1;
    radians result;
    int success = 1;


    result.val = acos_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }

    theta = -1;
    result.val = pi - acos_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }


    theta = 0;
    result.val = pi/2 - acos_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }


    theta = 0.540302305868;
    result.val =  1 - acos_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }

    theta = -0.416146836547;
    result.val =  2 - acos_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }

    theta = 0.200429529171;
    result.val =  1.369 - acos_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }

    if (success) {
        printf(GREEN "\t\tpassed " CHECK NRM "\n");
    } else {
        printf(RED "\t\tfailed " CROSS NRM "\n");
    }
} 

static void asin_radians_test(float eps) {
    printf("\n\t\tInverse Sin: \n");
    float theta = 0;
    radians result;
    int success = 1;


    result.val = asin_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }


    theta = 1;
    result.val = (pi/2) - asin_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }


    theta = -1;
    result.val = (pi/2) + asin_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }

    theta = 0.8414709848;
    result.val =  1 - asin_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }

    theta =  0.9092974268;
    result.val =  1.141591628 - asin_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }

    theta =  0.2002299847;
    result.val = 0.2015926536 - asin_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }

    if (success) {
        printf(GREEN "\t\tpassed " CHECK NRM "\n");
    } else {
        printf(RED "\t\tfailed " CROSS NRM "\n");
        exit(0);
    }
}

static void atan_radians_test(float eps) {
    printf("\n\t\tInverse Tan: \n");
    float theta = 0;
    radians result;
    int success = 1;

    result.val = atan_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }

    theta =  1;
    result.val =  (pi/4) - atan_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }

    theta = -1;
    result.val = pi/4 + atan_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }


    theta = 1.557407725;
    result.val =  1 - atan_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }


    theta =  -2.185039863;
    result.val = 1.141592677 + atan_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }


    theta = -0.1425465431;
    result.val =  0.1415926536 + atan_r(theta).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, theta);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, theta);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }


    if (success) {
        printf(GREEN "\t\tpassed " CHECK NRM "\n");
    } else {
        printf(RED "\t\tfailed " CROSS NRM "\n");
        exit(0);
    }
}

static void atan2_radians_test(float eps) {
    printf("\n\t\tAtan2: \n");
    float x = 1;
    float y = 0;
    radians result;
    int success = 1;

    result.val = atan2_r(y, x).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f,%f passed\n" NRM, x, y);
    } else {
        printf(RED "\t\t\tInput %f,%f failed\n" NRM, x, y);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }

    x = 1;
    y = 1;
    result.val =  pi/4 - atan2_r(y, x).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f,%f passed\n" NRM, x, y);
    } else {
        printf(RED "\t\t\tInput %f,%f failed\n" NRM, x, y);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }

    x = -1;
    y = -1;
    result.val = 3*pi/4 + atan2_r(y, x).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f,%f passed\n" NRM, x, y);
    } else {
        printf(RED "\t\t\tInput %f,%f failed\n" NRM, x, y);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }


    x = -1;
    y = 1;
    result.val =  3*pi/4 - atan2_r(y, x).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f,%f passed\n" NRM, x, y);
    } else {
        printf(RED "\t\t\tInput %f,%f failed\n" NRM, x, y);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }


    x = 1;
    y = -1;
    result.val = pi/4 + atan2_r(y, x).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f,%f passed\n" NRM, x, y);
    } else {
        printf(RED "\t\t\tInput %f,%f failed\n" NRM, x, y);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }


    x = 2;
    y = -1;
    result.val =  0.463647609001 + atan2_r(y, x).val;
    if (result.val <= eps && result.val >= -eps) {
        printf(GREEN "\t\t\tInput %f,%f passed\n" NRM, x, y);
    } else {
        printf(RED "\t\t\tInput %f,%f failed\n" NRM, x, y);
        printf("\t\t\t\tResult = %f\n", result.val);
        success = 0;
    }


    if (success) {
        printf(GREEN "\t\tpassed " CHECK NRM "\n");
    } else {
        printf(RED "\t\tfailed " CROSS NRM "\n");
        exit(0);
    }
}



static void test_trig() {
    float eps = 0.006;
    iterationCount = 14;
    printf("\tUsing EPS = %f\n", eps);
    printf("\tTesting Radians\n");
    cos_radians_test(eps);
    sin_radians_test(eps);
    tan_radians_test(eps);

    printf("\tTesting Degrees\n");
    cos_degrees_test(eps);
    sin_degrees_test(eps);
    tan_radians_test(eps);

    printf("\tTesting Inverses\n");
    acos_radians_test(eps);
    asin_radians_test(eps);
    atan_radians_test(eps);
    atan2_radians_test(eps);

    printf("Trig Tests... " GREEN "passed" CHECK NRM "\n");
}


#endif
