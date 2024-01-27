#ifndef SR_ERROR_H
#define SR_ERROR_H

typedef enum {
    SR_NO_ERROR = 0,
    SR_CREATE_FAIL,
    SR_EXTENSION_MISSING,
    
#ifdef DEBUG
    SR_VALIDATION_MISSING,
#endif

} ErrorCode;



#endif
