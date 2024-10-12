#ifndef SR_UTIL_H
#define SR_UTIL_H
#include <stdint.h>
#include <error.h>
#include "log.h"

typedef int bool;
#define TRUE 1
#define FALSE 0


#define DEF_OPTIONAL(type)\
    typedef struct { \
        bool exist; \
        type val; \
    } optional##type;


#define optional(type) optional##type
#define ARRAY_SIZE(x) sizeof( x )/sizeof( x[0] )

DEF_OPTIONAL(uint32_t);


#define PASS_CALL(f) \
    { \
        ErrorCode result = f; \
        if (result != SR_NO_ERROR) return result;\
    }

#define CRASH_CALL(f) \
    { \
        ErrorCode result = f; \
        if (result != SR_NO_ERROR) return EXIT_FAILURE;\
    }


#endif