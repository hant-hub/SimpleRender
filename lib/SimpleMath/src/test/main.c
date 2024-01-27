#include <stdio.h>
#include "logger.h"
#include "misc_test.h"
#include "vec_test.h"
#include "mat_test.h"
#include "trig_test.h"


int main() {

    test_vec();
    test_mat();
    test_trig();
    test_misc();
    return 0;
}
