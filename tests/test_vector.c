#include <emt/vector.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/*
 * Test yardımcıları.
 *
 * CHECK: koşul yanlışsa FAIL basar ve testin "cleanup" etiketine atlar.
 * Böylece her hata yolunda destroy'u elle tekrar yazmak gerekmez
 * (C'deki klasik goto-cleanup idiomu).
 *
 * Her test şu kalıbı izler:
 *
 *     int failed = 1;
 *     ... CHECK(...) ...
 *     failed = 0;
 * cleanup:
 *     emt_vec_destroy(&vector);
 *     return report(__func__, failed);
 */

#define CHECK(cond, msg)                                              \
    do {                                                              \
        if (!(cond)) {                                                \
            printf("[FAIL] %s:%d: %s\n", __func__, __LINE__, (msg));  \
            goto cleanup;                                             \
        }                                                             \
    } while (0)

static int report(const char *name, int failed)
{
    if (!failed) {
        printf("[PASS] %s\n", name);
    }
    return failed;
}

/* Bir vector'ü 1.5, 2.5, 3.5, ... ile doldurur (setter'dan bağımsız). */
static void fill_sequence(EmtVector *vector)
{
    for (size_t i = 0; i < vector->size; ++i) {
        vector->data[i] = 1.5 + (double)i;
    }
}

/* ================================================================
 * Lifecycle: init
 * ================================================================ */

static int test_init_success(void)
{
    EmtVector vector = {0};
    int failed = 1;

    CHECK(emt_vec_init(&vector, 10) == VEC_OK, "init failed");
    CHECK(vector.size == 10, "wrong size");
    CHECK(vector.data != NULL, "data is NULL");

    for (size_t i = 0; i < vector.size; ++i) {
        CHECK(vector.data[i] == 0.0, "element is not zero");
    }

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

static int test_init_size_one(void)
{
    EmtVector vector = {0};
    int failed = 1;

    CHECK(emt_vec_init(&vector, 1) == VEC_OK, "init failed");
    CHECK(vector.size == 1 && vector.data != NULL, "invalid state");
    CHECK(vector.data[0] == 0.0, "element is not zero");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

static int test_init_zero_size(void)
{
    EmtVector vector = {0};
    int failed = 1;

    CHECK(emt_vec_init(&vector, 0) == VEC_OK, "init(0) failed");
    CHECK(vector.size == 0 && vector.data == NULL, "not canonical empty");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

static int test_init_null_pointer(void)
{
    int failed = 1;

    CHECK(emt_vec_init(NULL, 10) == ERROR_NULL_PTR, "expected NULL_PTR");

    failed = 0;
cleanup:
    return report(__func__, failed);
}

static int test_init_existing_vector(void)
{
    EmtVector vector = {0};
    double *original_data;
    int failed = 1;

    CHECK(emt_vec_init(&vector, 4) == VEC_OK, "first init failed");
    original_data = vector.data;

    CHECK(emt_vec_init(&vector, 8) == ERROR_INVALID_STATE,
          "reinit of live vector not rejected");
    CHECK(vector.size == 4 && vector.data == original_data,
          "live vector was modified");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

/* {size > 0, data == NULL}: bozuk state, init reddetmeli, dokunmamalı. */
static int test_init_rejects_size_without_data(void)
{
    EmtVector vector = {3, NULL};
    int failed = 1;

    CHECK(emt_vec_init(&vector, 5) == ERROR_INVALID_STATE,
          "expected INVALID_STATE");
    CHECK(vector.size == 3 && vector.data == NULL, "state was modified");

    failed = 0;
cleanup:
    /* destroy güvenli: free(NULL) no-op */
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

/* {size == 0, data != NULL}: bozuk state. data bir stack değişkenini
 * gösteriyor; init ona yazmamalı ve onu free etmemeli.
 * Bu testte destroy ÇAĞRILMAZ: free(&local) invalid free olurdu. */
static int test_init_rejects_data_without_size(void)
{
    double local = 42.0;
    EmtVector vector = {0, &local};
    int failed = 1;

    CHECK(emt_vec_init(&vector, 5) == ERROR_INVALID_STATE,
          "expected INVALID_STATE");
    CHECK(vector.size == 0 && vector.data == &local, "state was modified");
    CHECK(local == 42.0, "foreign memory was written");

    failed = 0;
cleanup:
    return report(__func__, failed);
}

static int test_init_size_overflow(void)
{
    EmtVector vector = {0};
    size_t overflowing_size = SIZE_MAX / sizeof(double) + 1;
    int failed = 1;

    CHECK(emt_vec_init(&vector, overflowing_size) == ERROR_SIZE_OVERFLOW,
          "expected SIZE_OVERFLOW");
    CHECK(vector.size == 0 && vector.data == NULL, "state was modified");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

/*
 * Sınır değer: SIZE_MAX / sizeof(double) taşma DEĞİLDİR, fakat bu kadar
 * bellek ayrılamaz. Beklenen: ERROR_ALLOCATION ve vector değişmemiş.
 *
 * NOT: AddressSanitizer bu kadar büyük bir malloc isteğinde varsayılan
 * olarak abort eder. ASan build'inde şu ortam değişkeniyle çalıştır:
 *     ASAN_OPTIONS=allocator_may_return_null=1
 */
static int test_init_allocation_failure(void)
{
    EmtVector vector = {0};
    size_t largest_valid_size = SIZE_MAX / sizeof(double);
    int failed = 1;

    CHECK(emt_vec_init(&vector, largest_valid_size) == ERROR_ALLOCATION,
          "expected ALLOCATION (boundary must not count as overflow)");
    CHECK(vector.size == 0 && vector.data == NULL, "state was modified");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

/* ================================================================
 * Lifecycle: destroy
 * ================================================================ */

static int test_destroy_resets_vector(void)
{
    EmtVector vector = {0};
    int failed = 1;

    CHECK(emt_vec_init(&vector, 5) == VEC_OK, "init failed");
    emt_vec_destroy(&vector);
    CHECK(vector.size == 0 && vector.data == NULL, "not reset");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

static int test_destroy_is_safe(void)
{
    EmtVector vector = {0};
    int failed = 1;

    emt_vec_destroy(NULL);
    emt_vec_destroy(&vector);
    emt_vec_destroy(&vector);
    CHECK(vector.size == 0 && vector.data == NULL, "empty vector changed");

    failed = 0;
cleanup:
    return report(__func__, failed);
}

/* init → destroy → init: destroy gerçekten geçerli başlangıç state'ine
 * dönüyor mu? */
static int test_reinit_after_destroy(void)
{
    EmtVector vector = {0};
    int failed = 1;

    CHECK(emt_vec_init(&vector, 3) == VEC_OK, "first init failed");
    emt_vec_destroy(&vector);
    CHECK(emt_vec_init(&vector, 7) == VEC_OK, "reinit after destroy failed");
    CHECK(vector.size == 7 && vector.data != NULL, "invalid state");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

/* ================================================================
 * Getter: emt_vec_get_index
 * ================================================================ */

static int test_get_index_first_and_last(void)
{
    EmtVector vector = {0};
    double value = 0.0;
    int failed = 1;

    CHECK(emt_vec_init(&vector, 4) == VEC_OK, "init failed");
    fill_sequence(&vector);                     /* 1.5 2.5 3.5 4.5 */

    CHECK(emt_vec_get_index(&vector, &value, 0) == VEC_OK, "get(0) failed");
    CHECK(value == 1.5, "get(0) wrong value");

    CHECK(emt_vec_get_index(&vector, &value, 3) == VEC_OK, "get(last) failed");
    CHECK(value == 4.5, "get(last) wrong value");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

/* Dönen değer bir kopya: vector ölse de yaşar. */
static int test_get_index_returns_copy(void)
{
    EmtVector vector = {0};
    double value = 0.0;
    int failed = 1;

    CHECK(emt_vec_init(&vector, 2) == VEC_OK, "init failed");
    vector.data[1] = 7.25;

    CHECK(emt_vec_get_index(&vector, &value, 1) == VEC_OK, "get failed");
    vector.data[1] = -1.0;                      /* kaynağı değiştir */
    CHECK(value == 7.25, "value is not an independent copy");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

/* Sınır dışı: hata döner, out DEĞİŞMEZ. */
static int test_get_index_out_of_bounds(void)
{
    EmtVector vector = {0};
    double value = 99.0;
    int failed = 1;

    CHECK(emt_vec_init(&vector, 3) == VEC_OK, "init failed");

    CHECK(emt_vec_get_index(&vector, &value, 3) == ERROR_INDEX_OUT_OF_BOUNDS,
          "index == size not rejected");
    CHECK(value == 99.0, "out modified on failure");

    CHECK(emt_vec_get_index(&vector, &value, SIZE_MAX)
              == ERROR_INDEX_OUT_OF_BOUNDS,
          "SIZE_MAX index not rejected");
    CHECK(value == 99.0, "out modified on failure");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

static int test_get_index_empty_vector(void)
{
    EmtVector vector = {0};
    double value = 99.0;
    int failed = 1;

    CHECK(emt_vec_get_index(&vector, &value, 0) == ERROR_INDEX_OUT_OF_BOUNDS,
          "empty vector index 0 not rejected");
    CHECK(value == 99.0, "out modified on failure");

    failed = 0;
cleanup:
    return report(__func__, failed);
}

static int test_get_index_null_arguments(void)
{
    EmtVector vector = {0};
    double value = 99.0;
    int failed = 1;

    CHECK(emt_vec_init(&vector, 3) == VEC_OK, "init failed");

    CHECK(emt_vec_get_index(NULL, &value, 0) == ERROR_NULL_PTR,
          "NULL vector not rejected");
    CHECK(emt_vec_get_index(&vector, NULL, 0) == ERROR_NULL_PTR,
          "NULL out not rejected");
    CHECK(value == 99.0, "out modified on failure");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

static int test_get_index_invalid_state(void)
{
    EmtVector vector = {3, NULL};
    double value = 99.0;
    int failed = 1;

    CHECK(emt_vec_get_index(&vector, &value, 0) == ERROR_INVALID_STATE,
          "{size>0, data==NULL} not rejected");
    CHECK(value == 99.0, "out modified on failure");

    failed = 0;
cleanup:
    return report(__func__, failed);
}

/* ================================================================
 * Getter: emt_vec_get_array
 * ================================================================ */

static int test_get_array_exact_capacity(void)
{
    EmtVector vector = {0};
    double buffer[3] = {0.0, 0.0, 0.0};
    int failed = 1;

    CHECK(emt_vec_init(&vector, 3) == VEC_OK, "init failed");
    fill_sequence(&vector);                     /* 1.5 2.5 3.5 */

    CHECK(emt_vec_get_array(&vector, buffer, 3) == VEC_OK,
          "capacity == size rejected");
    CHECK(buffer[0] == 1.5 && buffer[1] == 2.5 && buffer[2] == 3.5,
          "wrong values copied");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

/* Kapasite fazlaysa: sadece size kadar yazılır, geri kalan dokunulmaz. */
static int test_get_array_larger_capacity(void)
{
    EmtVector vector = {0};
    double buffer[5] = {-1.0, -1.0, -1.0, -1.0, -1.0};
    int failed = 1;

    CHECK(emt_vec_init(&vector, 3) == VEC_OK, "init failed");
    fill_sequence(&vector);

    CHECK(emt_vec_get_array(&vector, buffer, 5) == VEC_OK, "copy failed");
    CHECK(buffer[2] == 3.5, "wrong values copied");
    CHECK(buffer[3] == -1.0 && buffer[4] == -1.0,
          "wrote past vector size");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

/* Buffer küçükse: hata döner, buffer'a HİÇ dokunulmaz (ya hep ya hiç). */
static int test_get_array_too_small(void)
{
    EmtVector vector = {0};
    double buffer[2] = {-1.0, -1.0};
    int failed = 1;

    CHECK(emt_vec_init(&vector, 3) == VEC_OK, "init failed");
    fill_sequence(&vector);

    CHECK(emt_vec_get_array(&vector, buffer, 2) == ERROR_BUFFER_TOO_SMALL,
          "small buffer not rejected");
    CHECK(buffer[0] == -1.0 && buffer[1] == -1.0,
          "partial write on failure");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

/* Kopya bağımsız mı: kopyalayıp vector'ü destroy et, buffer yaşamalı. */
static int test_get_array_survives_destroy(void)
{
    EmtVector vector = {0};
    double buffer[2] = {0.0, 0.0};
    int failed = 1;

    CHECK(emt_vec_init(&vector, 2) == VEC_OK, "init failed");
    fill_sequence(&vector);
    CHECK(emt_vec_get_array(&vector, buffer, 2) == VEC_OK, "copy failed");

    emt_vec_destroy(&vector);
    CHECK(buffer[0] == 1.5 && buffer[1] == 2.5,
          "buffer does not hold an independent copy");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

static int test_get_array_empty_vector(void)
{
    EmtVector vector = {0};
    double buffer[1] = {-1.0};
    int failed = 1;

    CHECK(emt_vec_get_array(&vector, buffer, 0) == VEC_OK,
          "empty vector into zero capacity failed");
    CHECK(buffer[0] == -1.0, "buffer modified");

    failed = 0;
cleanup:
    return report(__func__, failed);
}

static int test_get_array_null_arguments(void)
{
    EmtVector vector = {0};
    double buffer[3];
    int failed = 1;

    CHECK(emt_vec_init(&vector, 3) == VEC_OK, "init failed");

    CHECK(emt_vec_get_array(NULL, buffer, 3) == ERROR_NULL_PTR,
          "NULL vector not rejected");
    CHECK(emt_vec_get_array(&vector, NULL, 3) == ERROR_NULL_PTR,
          "NULL out not rejected");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

/* ================================================================
 * Setter: emt_vec_set_index
 * ================================================================ */

static int test_set_index_first_and_last(void)
{
    EmtVector vector = {0};
    int failed = 1;

    CHECK(emt_vec_init(&vector, 3) == VEC_OK, "init failed");

    CHECK(emt_vec_set_index(&vector, 0, -2.5) == VEC_OK, "set(0) failed");
    CHECK(emt_vec_set_index(&vector, 2, 8.0) == VEC_OK, "set(last) failed");

    CHECK(vector.data[0] == -2.5, "set(0) wrong value");
    CHECK(vector.data[1] == 0.0, "untouched element changed");
    CHECK(vector.data[2] == 8.0, "set(last) wrong value");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

/* Sınır dışı: hata döner, vector'ün HİÇBİR elemanı değişmez. */
static int test_set_index_out_of_bounds(void)
{
    EmtVector vector = {0};
    int failed = 1;

    CHECK(emt_vec_init(&vector, 3) == VEC_OK, "init failed");
    fill_sequence(&vector);                     /* 1.5 2.5 3.5 */

    CHECK(emt_vec_set_index(&vector, 3, 100.0) == ERROR_INDEX_OUT_OF_BOUNDS,
          "index == size not rejected");
    CHECK(emt_vec_set_index(&vector, SIZE_MAX, 100.0)
              == ERROR_INDEX_OUT_OF_BOUNDS,
          "SIZE_MAX index not rejected");

    CHECK(vector.size == 3, "size changed");
    CHECK(vector.data[0] == 1.5 && vector.data[1] == 2.5 &&
          vector.data[2] == 3.5,
          "vector modified on failure");

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

static int test_set_index_empty_vector(void)
{
    EmtVector vector = {0};
    int failed = 1;

    CHECK(emt_vec_set_index(&vector, 0, 1.0) == ERROR_INDEX_OUT_OF_BOUNDS,
          "empty vector index 0 not rejected");
    CHECK(vector.size == 0 && vector.data == NULL, "empty vector changed");

    failed = 0;
cleanup:
    return report(__func__, failed);
}

static int test_set_index_null_and_invalid(void)
{
    EmtVector broken = {3, NULL};
    int failed = 1;

    CHECK(emt_vec_set_index(NULL, 0, 1.0) == ERROR_NULL_PTR,
          "NULL vector not rejected");
    CHECK(emt_vec_set_index(&broken, 0, 1.0) == ERROR_INVALID_STATE,
          "{size>0, data==NULL} not rejected");

    failed = 0;
cleanup:
    return report(__func__, failed);
}

/* set → get tur testi: setter ile getter birbiriyle tutarlı mı? */
static int test_set_then_get_roundtrip(void)
{
    EmtVector vector = {0};
    double value = 0.0;
    int failed = 1;

    CHECK(emt_vec_init(&vector, 5) == VEC_OK, "init failed");

    for (size_t i = 0; i < vector.size; ++i) {
        CHECK(emt_vec_set_index(&vector, i, (double)i * 0.5) == VEC_OK,
              "set failed");
    }
    for (size_t i = 0; i < vector.size; ++i) {
        CHECK(emt_vec_get_index(&vector, &value, i) == VEC_OK, "get failed");
        CHECK(value == (double)i * 0.5, "roundtrip value mismatch");
    }

    failed = 0;
cleanup:
    emt_vec_destroy(&vector);
    return report(__func__, failed);
}

/* ================================================================
 * Fonksiyon tablosu: EmtVecOps
 * ================================================================ */

static int test_ops_table_points_to_core(void)
{
    int failed = 1;

    CHECK(EmtVecOps.init == emt_vec_init, "init mismatch");
    CHECK(EmtVecOps.destroy == emt_vec_destroy, "destroy mismatch");
    CHECK(EmtVecOps.getIndex == emt_vec_get_index, "getIndex mismatch");
    CHECK(EmtVecOps.getArray == emt_vec_get_array, "getArray mismatch");
    CHECK(EmtVecOps.setIndex == emt_vec_set_index, "setIndex mismatch");

    failed = 0;
cleanup:
    return report(__func__, failed);
}

static int test_ops_table_usage(void)
{
    EmtVector vector = {0};
    double value = 0.0;
    int failed = 1;

    CHECK(EmtVecOps.init(&vector, 2) == VEC_OK, "init via table failed");
    CHECK(EmtVecOps.setIndex(&vector, 1, 6.5) == VEC_OK,
          "setIndex via table failed");
    CHECK(EmtVecOps.getIndex(&vector, &value, 1) == VEC_OK,
          "getIndex via table failed");
    CHECK(value == 6.5, "wrong value via table");

    failed = 0;
cleanup:
    EmtVecOps.destroy(&vector);
    return report(__func__, failed);
}

/* ================================================================ */

int main(void)
{
    int failures = 0;

    /* lifecycle */
    failures += test_init_success();
    failures += test_init_size_one();
    failures += test_init_zero_size();
    failures += test_init_null_pointer();
    failures += test_init_existing_vector();
    failures += test_init_rejects_size_without_data();
    failures += test_init_rejects_data_without_size();
    failures += test_init_size_overflow();
    failures += test_init_allocation_failure();
    failures += test_destroy_resets_vector();
    failures += test_destroy_is_safe();
    failures += test_reinit_after_destroy();

    /* get_index */
    failures += test_get_index_first_and_last();
    failures += test_get_index_returns_copy();
    failures += test_get_index_out_of_bounds();
    failures += test_get_index_empty_vector();
    failures += test_get_index_null_arguments();
    failures += test_get_index_invalid_state();

    /* get_array */
    failures += test_get_array_exact_capacity();
    failures += test_get_array_larger_capacity();
    failures += test_get_array_too_small();
    failures += test_get_array_survives_destroy();
    failures += test_get_array_empty_vector();
    failures += test_get_array_null_arguments();

    /* set_index */
    failures += test_set_index_first_and_last();
    failures += test_set_index_out_of_bounds();
    failures += test_set_index_empty_vector();
    failures += test_set_index_null_and_invalid();
    failures += test_set_then_get_roundtrip();

    /* ops table */
    failures += test_ops_table_points_to_core();
    failures += test_ops_table_usage();

    if (failures != 0) {
        printf("[SUMMARY] %d test(s) failed\n", failures);
        return 1;
    }

    printf("[SUMMARY] all vector tests passed\n");
    return 0;
}
