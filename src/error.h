#ifndef ERROR_H
#define ERROR_H
#include "errno.h"

extern int errno;


#define ERR_COLOR(x)   "\033[38;2;255;0;0m"   x "\033[0m\n"
#define WARN_COLOR(x)  "\033[38;2;255;255;0m" x "\033[0m\n"
#define DEBUG_COLOR(x) "\033[38;2;255;0;255m" x "\033[0m\n"
#define TRACE_COLOR(x) x "\n"

typedef enum ErrorCodes {
    NoError = 0,
    FailedCreation,
    NoValidationSupport,
    NoExtension,
    FailedSearch
} ErrorCodes;


#endif
