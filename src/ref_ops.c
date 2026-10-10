#include "ops.h"

#include <string.h>

int64_t ref_add_i64(int64_t left, int64_t right) {
    uint64_t wrapped = (uint64_t)left + (uint64_t)right;
    int64_t result = 0;
    memcpy(&result, &wrapped, sizeof result);
    return result;
}

int64_t ref_sum_i64(const int64_t *values, size_t count) {
    uint64_t sum = 0;
    size_t index;
    int64_t result = 0;
    if (values == NULL || count == 0u) {
        return 0;
    }
    for (index = 0; index < count; ++index) {
        uint64_t term = 0;
        memcpy(&term, &values[index], sizeof term);
        sum += term;
    }
    memcpy(&result, &sum, sizeof result);
    return result;
}

int64_t ref_max_i64(const int64_t *values, size_t count) {
    int64_t max_value;
    size_t index;
    max_value = values[0];
    for (index = 1; index < count; ++index) {
        if (values[index] > max_value) {
            max_value = values[index];
        }
    }
    return max_value;
}

uint64_t ref_factorial_u64(uint64_t n, int *overflow) {
    uint64_t product = 1;
    uint64_t factor;
    *overflow = 0;
    if (n <= 1u) {
        return 1u;
    }
    for (factor = 2; factor <= n; ++factor) {
        if (product > UINT64_MAX / factor) {
            *overflow = 1;
            return 0u;
        }
        product *= factor;
    }
    return product;
}
