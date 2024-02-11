#ifndef SR_ERROR_H
#define SR_ERROR_H

typedef enum {
    SR_NO_ERROR = 0,
    SR_CREATE_FAIL,
    SR_EXTENSION_MISSING,
    SR_DEVICE_NOT_FOUND,
    SR_INVALID,
    
#ifdef DEBUG
    SR_VALIDATION_MISSING,
#endif

} ErrorCode;



#endif
