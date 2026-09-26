#ifndef EMT_VECTOR_H
#define EMT_VECTOR_H

#include <stddef.h>

#define EMT_TRY(expr)                        \
    do {                                     \
        EmtVectorStatus emt_s_ = (expr);     \
        if (emt_s_ != VEC_OK) return emt_s_; \
    } while (0)

#define EMT_CHECK_NULL(p)                    \
    do {                                     \
        if ((p) == NULL) return ERROR_NULL_PTR; \
    } while (0)

typedef enum {
    VEC_OK = 0,
    ERROR_NULL_PTR = -1,
    ERROR_INVALID_STATE = -2,
    ERROR_SIZE_OVERFLOW = -3,
    ERROR_ALLOCATION = -4,
    ERROR_INDEX_OUT_OF_BOUNDS = -5,
    ERROR_BUFFER_TOO_SMALL = -6,
    ERROR_SIZE_MISMATCH = -7
} EmtVectorStatus;
typedef struct {
    size_t size;
    double * data;
} EmtVector;

EmtVectorStatus emt_vec_init(EmtVector *vector, size_t size);
void emt_vec_destroy(EmtVector *vector);

/** getters and setters */
EmtVectorStatus emt_vec_get_array(const EmtVector* vector, double * out, size_t capacity);

EmtVectorStatus emt_vec_get_index(const EmtVector* vector, double * out, size_t index);

EmtVectorStatus emt_vec_set_index(EmtVector* vector, size_t index, double value);

EmtVectorStatus emt_vec_deep_copy(EmtVector* dst, const EmtVector* const src);
//---- Arithmetic ----
/** this supports aliasing. */
EmtVectorStatus emt_vec_add(EmtVector* out, const EmtVector* a, const EmtVector* b);
/** this supports aliasing. */
EmtVectorStatus emt_vec_sub(EmtVector* out, const EmtVector* a, const EmtVector* b);

EmtVectorStatus emt_vec_fill(EmtVector* dst, double value);
/** this supports aliasing. */
EmtVectorStatus emt_vec_scale(EmtVector* out, const EmtVector* src, double scalar);
/** this supports aliasing. */
EmtVectorStatus emt_vec_dot(double* out, const EmtVector* a, const EmtVector* b);


typedef struct {
    EmtVectorStatus (*init)(EmtVector *vector, size_t size);
    void (*destroy)(EmtVector *vector);
    EmtVectorStatus (*getIndex)(const EmtVector* vector, double * out, size_t index);
    EmtVectorStatus (*getArray)(const EmtVector* vector, double * out, size_t capacity);
    EmtVectorStatus (*setIndex)(EmtVector* vector, size_t index, double value);
    EmtVectorStatus (*copy)(EmtVector* dst, const EmtVector* const src);
    EmtVectorStatus (*add)(EmtVector* out, const EmtVector* a, const EmtVector* b);
    EmtVectorStatus (*sub)(EmtVector* out, const EmtVector* a, const EmtVector* b);
    EmtVectorStatus (*fill)(EmtVector* dst, double value);
    EmtVectorStatus (*scale)(EmtVector* out, const EmtVector* src, double scalar);

} EmtVectorApi;

extern const EmtVectorApi EmtVecOps;

#endif
/*EMT_VECTOR_H */
