#ifndef CS2521_LOCALITY_H
#define CS2521_LOCALITY_H

#include <stddef.h>
#include <stdint.h>

/*
 * Row-major and column-major summations of an int64 matrix stored row-major.
 * Integer addition is associative, so both traversals produce the same sum.
 * The traversal order is what changes cache behavior.
 *
 * locality_measure_* writes the sum and the elapsed monotonic seconds.
 * Returns 0 on success and -1 on a bad argument or a clock failure.
 * A negative elapsed time is not produced on success.
 */

int64_t locality_sum_row_major(const int64_t *matrix, size_t rows, size_t cols);
int64_t locality_sum_column_major(const int64_t *matrix, size_t rows, size_t cols);

int locality_measure_row_major(const int64_t *matrix, size_t rows, size_t cols,
                               int64_t *sum, double *seconds);
int locality_measure_column_major(const int64_t *matrix, size_t rows, size_t cols,
                                  int64_t *sum, double *seconds);

#endif
