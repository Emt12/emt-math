#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <emt/vector.h>

VectorStatus initVector(Vector *vector, size_t size) {
    if (vector == NULL) {
        return ERROR_NULL_PTR;
    }
    
    if (vector->size != 0 || vector->data != NULL) {
        return ERROR_INVALID_STATE;
    }

    if (size == 0) {
        return OK;
    }
    
    double* tempData;
    
    if (size > (SIZE_MAX / sizeof(*tempData))) {
        return ERROR_SIZE_OVERFLOW;
    }

    tempData = malloc(size * sizeof(*tempData));
    
    if (tempData == NULL) {
        return ERROR_ALLOCATION;
    }
    
    for (size_t i = 0; i < size; ++i) {
        tempData[i] = 0.0;
    }
    
    vector->data = tempData;
    vector->size = size;

    return OK;
}

void destroyVector(Vector *vector) {
    if (vector == NULL) {
        return;
    }
    
    free(vector->data);
    vector->data = NULL;
    vector->size = 0;
}
