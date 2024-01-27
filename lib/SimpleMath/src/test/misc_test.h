#ifndef MISC_TEST_H
#define MISC_TEST_H
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/misc/sqrt.h"

static void test_sqrt(float eps) {
    printf("\tTesting Sqrt:\n");
    int success = 1;

    float value = 1;
    float guess = sqrt(value);
    float error = guess*guess - value;
    if (error < eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, value);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, value);
        printf("\t\t\t\tResult = %f\n", guess);
        success = 0;
    }

    value = 4;
    guess = sqrt(value);
    error = guess*guess - value;
    if (error < eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, value);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, value);
        printf("\t\t\t\tResult = %f\n", guess);
        success = 0;
    }


    value = 2;
    guess = sqrt(value);
    error = guess*guess - value;
    if (error < eps) {
        printf(GREEN "\t\t\tInput %f passed\n" NRM, value);
    } else {
        printf(RED "\t\t\tInput %f failed\n" NRM, value);
        printf("\t\t\t\tResult = %f\n", guess);
        success = 0;
    }


    for (int i = 1; i<10; i++) {
        value = 8.92938295 * (float)i;
        guess = sqrt(value);
        error = guess*guess - value;
        if (error < eps) {
            printf(GREEN "\t\t\tInput %f passed\n" NRM, value);
        } else {
            printf(RED "\t\t\tInput %f failed\n" NRM, value);
            printf("\t\t\t\tResult = %f\n", guess);
            success = 0;
        }
    }




    if (success == 0) {
        printf(RED "Tests Failed" CROSS NRM "\n");
        exit(0);
    } 
    printf(GREEN "\tTests Passed" CHECK NRM "\n");
    
}



static void test_misc() {
    float eps = 0.0001;
    printf("\tUsing EPS = %f\n", eps);
    
    test_sqrt(eps);

    printf("Misc Tests... " GREEN "passed" CHECK NRM "\n");
}

#endif
