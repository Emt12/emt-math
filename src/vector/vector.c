#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <emt/vector.h>

static EmtVectorStatus check_vector(const EmtVector *v)
{
    EMT_CHECK_NULL(v);
    if (v->size > 0 && v->data == NULL) {
        return ERROR_INVALID_STATE;
    }
    return VEC_OK;
}

/* ---- Lifecycle ---- */

EmtVectorStatus emt_vec_init(EmtVector *vector, size_t size)
{
    EMT_CHECK_NULL(vector);

    /* init sadece EMPTY vector kabul eder; check_vector burada uygun değil */
    if (vector->size != 0 || vector->data != NULL) {
        return ERROR_INVALID_STATE;
    }

    if (size == 0) {
        return VEC_OK;
    }

    if (size > SIZE_MAX / sizeof(double)) {
        return ERROR_SIZE_OVERFLOW;
    }

    double *tempData = malloc(size * sizeof *tempData);
    if (tempData == NULL) {
        return ERROR_ALLOCATION;
    }

    for (size_t i = 0; i < size; ++i) {
        tempData[i] = 0.0;
    }

    vector->data = tempData;
    vector->size = size;
    return VEC_OK;
}

void emt_vec_destroy(EmtVector *vector)
{
    if (vector == NULL) {        /* void fonksiyon: macro kullanılamaz */
        return;
    }

    free(vector->data);
    vector->data = NULL;
    vector->size = 0;
}

/* ---- Getters / setters ---- */

EmtVectorStatus emt_vec_get_index(const EmtVector *vector,
                                  double *out,
                                  size_t index)
{
    EMT_TRY(check_vector(vector));
    EMT_CHECK_NULL(out);

    if (index >= vector->size) {
        return ERROR_INDEX_OUT_OF_BOUNDS;
    }

    *out = vector->data[index];
    return VEC_OK;
}

EmtVectorStatus emt_vec_get_array(const EmtVector *vector,
                                  double *out,
                                  size_t capacity)
{
    EMT_TRY(check_vector(vector));
    EMT_CHECK_NULL(out);

    if (vector->size > capacity) {
        return ERROR_BUFFER_TOO_SMALL;   /* out'a hiç dokunulmadı */
    }

    /* her şey doğrulandı: döngüde hata çıkamaz, yarım yazma riski yok */
    for (size_t i = 0; i < vector->size; ++i) {
        out[i] = vector->data[i];
    }
    return VEC_OK;
}

EmtVectorStatus emt_vec_set_index(EmtVector *vector,   /* const DEĞİL */
                                  size_t index,
                                  double value)
{  
    EMT_TRY(check_vector(vector));
    EMT_CHECK_NULL(vector);

    if (index >= vector->size) {
        return ERROR_INDEX_OUT_OF_BOUNDS;
    }

    vector->data[index] = value;
    
    return VEC_OK;
}

EmtVectorStatus emt_vec_deep_copy(
    EmtVector* dst,
    const EmtVector* const src
) {
    EMT_TRY(check_vector(dst));
    EMT_TRY(check_vector(src));

    if(dst->size != src->size)
        return ERROR_SIZE_MISMATCH;
    
    for (size_t i = 0; i < src->size; ++i)
        dst->data[i] = src->data[i];
    
    return VEC_OK;
}

/* ---- Arithmetic ----- */
EmtVectorStatus emt_vec_add(
    EmtVector* out,
    const EmtVector* a, const EmtVector* b) {
        EMT_TRY(check_vector(a));
        EMT_TRY(check_vector(b));
        EMT_TRY(check_vector(out));
        if (a->size != b->size || out->size != a->size)
            return ERROR_SIZE_MISMATCH;
        
        for (size_t i = 0; i < a->size; ++i) {
            out->data[i] = a->data[i] + b->data[i];
        }
        return VEC_OK;
    }

EmtVectorStatus emt_vec_sub(
    EmtVector* out,
    const EmtVector* a, const EmtVector* b) {
        EMT_TRY(check_vector(a));
        EMT_TRY(check_vector(b));
        EMT_TRY(check_vector(out));
        if (a->size != b->size || out->size != a->size)
            return ERROR_SIZE_MISMATCH;
        
        for (size_t i = 0; i < a->size; ++i) {
            out->data[i] = a->data[i] - b->data[i];
        }
        return VEC_OK;
    }

EmtVectorStatus emt_vec_fill(EmtVector* dst, double value) {
    EMT_TRY(check_vector(dst));
    for (size_t i = 0; i < dst->size; ++i)
        dst->data[i] = value;
    return VEC_OK;
}

EmtVectorStatus emt_vec_scale(EmtVector* out, const EmtVector* src, double scalar) {
    EMT_TRY(check_vector(out));
    EMT_TRY(check_vector(src));
    if(out->size != src->size)
        return ERROR_SIZE_MISMATCH;
    for (size_t i = 0; i < out->size; ++i) {
        out->data[i] = src->data[i] * scalar;
    }
}

EmtVectorStatus emt_vec_dot(double* out, const EmtVector* a, const EmtVector* b) {
    EMT_CHECK_NULL(out);
    EMT_TRY(check_vector(a));
    EMT_TRY(check_vector(b));

    if(a->size != b->size)
        return ERROR_SIZE_MISMATCH;
    double sum = 0.0;
    for (size_t i = 0; i < a->size; ++i) {
        sum += a->data[i] * b->data[i];
    }
    *out = sum;
    return VEC_OK;
}

/* ---- Fonksiyon tablosu ---- */

const EmtVectorApi EmtVecOps = {
    .init     = emt_vec_init,
    .destroy  = emt_vec_destroy,
    .getIndex = emt_vec_get_index,
    .getArray = emt_vec_get_array,
    .setIndex = emt_vec_set_index,
    .add = emt_vec_add,
    .copy = emt_vec_deep_copy,
    .sub = emt_vec_sub,
    .fill = emt_vec_fill,
    .scale = emt_vec_scale
};
