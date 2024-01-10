#ifndef OPTION_H
#define OPTION_H

#define DEF_OPTIONAL(type) \
    typedef struct optional_##type { \
        unsigned int has_value : 1; \
        type value; \
    } optional_##type;

#define optional(type) optional_##type 







#endif
