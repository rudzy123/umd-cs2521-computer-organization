#include "ops.h"
#include "test_harness.h"

#include <stdint.h>

static void test_add(void) {
    EXPECT_LL(asm_add_i64(2, 3), 5);
    EXPECT_LL(asm_add_i64(-4, 10), 6);
    EXPECT_LL(asm_add_i64(-3, -5), -8);
    EXPECT_LL(asm_add_i64(INT64_MAX, 1), INT64_MIN);
    EXPECT_LL(asm_add_i64(INT64_MAX, 1), ref_add_i64(INT64_MAX, 1));
    EXPECT_LL(asm_add_i64(INT64_MIN, -1), INT64_MAX);
    EXPECT_LL(asm_add_i64(INT64_MIN, -1), ref_add_i64(INT64_MIN, -1));
    EXPECT_LL(asm_add_i64(0, 0), ref_add_i64(0, 0));
}

static void test_sum(void) {
    const int64_t rising[] = {1, 2, 3, 4};
    const int64_t mixed[] = {-8, 5, -1, 4};
    const int64_t wrapped[] = {INT64_MAX, 1};
    const int64_t negatives[] = {-1, -1};

    EXPECT_LL(asm_sum_i64(NULL, 0), 0);
    EXPECT_LL(ref_sum_i64(NULL, 0), 0);
    EXPECT_LL(asm_sum_i64(rising, 0), 0);
    EXPECT_LL(asm_sum_i64(rising, 4), 10);
    EXPECT_LL(asm_sum_i64(rising, 4), ref_sum_i64(rising, 4));
    EXPECT_LL(asm_sum_i64(mixed, 4), ref_sum_i64(mixed, 4));
    EXPECT_LL(asm_sum_i64(mixed, 4), 0);
    EXPECT_LL(asm_sum_i64(wrapped, 2), INT64_MIN);
    EXPECT_LL(asm_sum_i64(wrapped, 2), ref_sum_i64(wrapped, 2));
    EXPECT_LL(asm_sum_i64(negatives, 2), -2);
    EXPECT_LL(asm_sum_i64(negatives, 2), ref_sum_i64(negatives, 2));
}

static void test_max(void) {
    const int64_t values[] = {INT64_MIN, -1, INT64_MAX, 0};
    const int64_t negatives[] = {-5, -1, -8};
    const int64_t ties[] = {5, 5, 4};
    const int64_t alone[] = {INT64_MIN};

    EXPECT_LL(asm_max_i64(values, 4), INT64_MAX);
    EXPECT_LL(asm_max_i64(values, 4), ref_max_i64(values, 4));
    EXPECT_LL(asm_max_i64(negatives, 3), -1);
    EXPECT_LL(asm_max_i64(negatives, 3), ref_max_i64(negatives, 3));
    EXPECT_LL(asm_max_i64(ties, 3), 5);
    EXPECT_LL(asm_max_i64(ties, 3), ref_max_i64(ties, 3));
    EXPECT_LL(asm_max_i64(alone, 1), INT64_MIN);
    EXPECT_LL(asm_max_i64(alone, 1), ref_max_i64(alone, 1));
}

static void expect_factorial(uint64_t n, unsigned long long want, int expect_overflow) {
    int asm_overflow = 9;
    int ref_overflow = 9;
    uint64_t asm_value = asm_factorial_u64(n, &asm_overflow);
    uint64_t ref_value = ref_factorial_u64(n, &ref_overflow);
    EXPECT(asm_overflow == expect_overflow);
    EXPECT(ref_overflow == expect_overflow);
    EXPECT_ULL(asm_value, ref_value);
    if (!expect_overflow) {
        EXPECT_ULL(asm_value, want);
    } else {
        EXPECT_ULL(asm_value, 0);
    }
}

static void test_factorial(void) {
    static const unsigned long long k_small[] = {
        1ull, 1ull, 2ull, 6ull, 24ull, 120ull, 720ull, 5040ull, 40320ull, 362880ull, 3628800ull};
    uint64_t n;
    for (n = 0; n < sizeof k_small / sizeof k_small[0]; ++n) {
        expect_factorial(n, k_small[n], 0);
    }
    expect_factorial(20, 2432902008176640000ull, 0);
    expect_factorial(21, 0, 1);
    expect_factorial(22, 0, 1);
    expect_factorial(40, 0, 1);
}

int main(void) {
    test_add();
    test_sum();
    test_max();
    test_factorial();
    return test_finish();
}
