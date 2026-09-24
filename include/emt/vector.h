#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

typedef enum {
    OK = 0,
    ERROR_NULL_PTR = -1,
    ERROR_INVALID_STATE = -2,
    ERROR_SIZE_OVERFLOW = -3,
    ERROR_ALLOCATION = -4
} VectorStatus;

typedef struct {
    size_t size;
    double *data;
} Vector;

VectorStatus initVector(Vector *vector, size_t size);
void destroyVector(Vector *vector);

#endif
/*EMT_VECTOR_H */
