#include <emt/vector.h>

#include <stdint.h>
#include <stdio.h>

static int test_init_success(void)
{
    Vector vector = {0};
    VectorStatus status = initVector(&vector, 10);

    if (status != OK) {
        printf("[FAIL] test_init_success: status = %d\n", status);
        return 1;
    }

    if (vector.size != 10 || vector.data == NULL) {
        printf("[FAIL] test_init_success: invalid vector state\n");
        destroyVector(&vector);
        return 1;
    }

    for (size_t i = 0; i < vector.size; ++i) {
        if (vector.data[i] != 0.0) {
            printf("[FAIL] test_init_success: data[%zu] is not zero\n", i);
            destroyVector(&vector);
            return 1;
        }
    }

    destroyVector(&vector);
    printf("[PASS] test_init_success\n");
    return 0;
}

static int test_init_zero_size(void)
{
    Vector vector = {0};
    VectorStatus status = initVector(&vector, 0);

    if (status != OK || vector.size != 0 || vector.data != NULL) {
        printf("[FAIL] test_init_zero_size\n");
        destroyVector(&vector);
        return 1;
    }

    destroyVector(&vector);
    printf("[PASS] test_init_zero_size\n");
    return 0;
}

static int test_init_null_pointer(void)
{
    if (initVector(NULL, 10) != ERROR_NULL_PTR) {
        printf("[FAIL] test_init_null_pointer\n");
        return 1;
    }

    printf("[PASS] test_init_null_pointer\n");
    return 0;
}

static int test_init_existing_vector(void)
{
    Vector vector = {0};
    VectorStatus first_status = initVector(&vector, 4);

    if (first_status != OK) {
        printf("[FAIL] test_init_existing_vector: first init failed\n");
        return 1;
    }

    double *original_data = vector.data;
    VectorStatus second_status = initVector(&vector, 8);

    if (second_status != ERROR_INVALID_STATE ||
        vector.size != 4 ||
        vector.data != original_data) {
        printf("[FAIL] test_init_existing_vector: live vector was modified\n");
        destroyVector(&vector);
        return 1;
    }

    destroyVector(&vector);
    printf("[PASS] test_init_existing_vector\n");
    return 0;
}

static int test_init_size_overflow(void)
{
    Vector vector = {0};
    size_t overflowing_size = SIZE_MAX / sizeof(double) + 1;
    VectorStatus status = initVector(&vector, overflowing_size);

    if (status != ERROR_SIZE_OVERFLOW ||
        vector.size != 0 ||
        vector.data != NULL) {
        printf("[FAIL] test_init_size_overflow\n");
        destroyVector(&vector);
        return 1;
    }

    printf("[PASS] test_init_size_overflow\n");
    return 0;
}

static int test_destroy_resets_vector(void)
{
    Vector vector = {0};

    if (initVector(&vector, 5) != OK) {
        printf("[FAIL] test_destroy_resets_vector: init failed\n");
        return 1;
    }

    destroyVector(&vector);

    if (vector.size != 0 || vector.data != NULL) {
        printf("[FAIL] test_destroy_resets_vector\n");
        return 1;
    }

    printf("[PASS] test_destroy_resets_vector\n");
    return 0;
}

static int test_destroy_is_safe(void)
{
    Vector vector = {0};

    destroyVector(NULL);
    destroyVector(&vector);
    destroyVector(&vector);

    if (vector.size != 0 || vector.data != NULL) {
        printf("[FAIL] test_destroy_is_safe\n");
        return 1;
    }

    printf("[PASS] test_destroy_is_safe\n");
    return 0;
}

int main(void)
{
    int failures = 0;

    failures += test_init_success();
    failures += test_init_zero_size();
    failures += test_init_null_pointer();
    failures += test_init_existing_vector();
    failures += test_init_size_overflow();
    failures += test_destroy_resets_vector();
    failures += test_destroy_is_safe();

    if (failures != 0) {
        printf("[SUMMARY] %d test(s) failed\n", failures);
        return 1;
    }

    printf("[SUMMARY] all vector lifecycle tests passed\n");
    return 0;
}
