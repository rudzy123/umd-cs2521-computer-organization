#define _POSIX_C_SOURCE 200809L

#include "locality.h"

#include <time.h>

static int arguments_ok(const int64_t *matrix, size_t rows, size_t cols) {
    if (cols != 0u && rows > SIZE_MAX / cols) {
        return 0;
    }
    if (rows != 0u && cols != 0u && matrix == NULL) {
        return 0;
    }
    return 1;
}

int64_t locality_sum_row_major(const int64_t *matrix, size_t rows, size_t cols) {
    int64_t sum = 0;
    size_t row;
    if (!arguments_ok(matrix, rows, cols)) {
        return 0;
    }
    for (row = 0; row < rows; ++row) {
        size_t col;
        size_t base = row * cols;
        for (col = 0; col < cols; ++col) {
            sum += matrix[base + col];
        }
    }
    return sum;
}

int64_t locality_sum_column_major(const int64_t *matrix, size_t rows, size_t cols) {
    int64_t sum = 0;
    size_t col;
    if (!arguments_ok(matrix, rows, cols)) {
        return 0;
    }
    for (col = 0; col < cols; ++col) {
        size_t row;
        for (row = 0; row < rows; ++row) {
            sum += matrix[row * cols + col];
        }
    }
    return sum;
}

static int measure(int64_t (*sum_fn)(const int64_t *, size_t, size_t),
                   const int64_t *matrix, size_t rows, size_t cols,
                   int64_t *sum, double *seconds) {
    struct timespec start;
    struct timespec stop;
    if (sum == NULL || seconds == NULL || sum_fn == NULL) {
        return -1;
    }
    if (!arguments_ok(matrix, rows, cols)) {
        return -1;
    }
    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        return -1;
    }
    {
        int64_t computed = sum_fn(matrix, rows, cols);
        double elapsed;
        if (clock_gettime(CLOCK_MONOTONIC, &stop) != 0) {
            return -1;
        }
        elapsed = (double)(stop.tv_sec - start.tv_sec) +
                  (double)(stop.tv_nsec - start.tv_nsec) / 1000000000.0;
        if (elapsed < 0.0) {
            return -1;
        }
        *sum = computed;
        *seconds = elapsed;
    }
    return 0;
}

int locality_measure_row_major(const int64_t *matrix, size_t rows, size_t cols,
                               int64_t *sum, double *seconds) {
    return measure(locality_sum_row_major, matrix, rows, cols, sum, seconds);
}

int locality_measure_column_major(const int64_t *matrix, size_t rows, size_t cols,
                                  int64_t *sum, double *seconds) {
    return measure(locality_sum_column_major, matrix, rows, cols, sum, seconds);
}
