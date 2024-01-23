#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include <errno.h>

typedef struct SizedBuffer {
    size_t size;
    char* buffer;
} SizedBuffer;

extern int errno;

// Buffer is allocated with malloc, must be manually freed
static void LoadFile(const char* filename, SizedBuffer* buffer) {
    errno = 0;
    FILE* file = fopen(filename, "rb"); 
    if (file == NULL) {
        fprintf(stderr, ERR_COLOR("Failed to Open File"));
        return;
    }

    fseek(file, 0, SEEK_END);
    buffer->size = (size_t)ftell(file);
    fseek(file, 0, SEEK_SET); 

    buffer->buffer = (char *)aligned_alloc(sizeof(uint32_t), buffer->size*sizeof(char));


    if (buffer == NULL) {
        fprintf(stderr, ERR_COLOR("Malloc failed somehow. Fuck"));
        fclose(file);
        return;
    }

    if (fread(buffer->buffer, sizeof(char), buffer->size, file) != buffer->size) {
        fprintf(stderr, ERR_COLOR("Failed to Read entire file"));
        fclose(file);
        return;
    }

    if (errno != 0) {
        perror("readfile func");
    }

    fclose(file);
    return;
}

#endif
