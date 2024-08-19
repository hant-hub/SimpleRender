#ifndef SR_LOG_H
#define SR_LOG_H


#ifdef DEBUG
#include <stdio.h>

#define SR_LOG_ERR(x, ...) fprintf(stderr, "\033[38;2;255;0;0m" x "\033[0m\n", ##__VA_ARGS__)
#define SR_LOG_WAR(x, ...) fprintf(stdout, "\033[38;2;255;255;0m" x "\033[0m\n", ##__VA_ARGS__)
#define SR_LOG_DEB(x, ...) fprintf(stdout, x "\n", ##__VA_ARGS__)

#else

#define SR_LOG_ERR(x)
#define SR_LOG_WAR(x)
#define SR_LOG_DEB(x)

#endif




#endif
