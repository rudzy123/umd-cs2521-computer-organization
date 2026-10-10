#include "locality.h"
#include "test_harness.h"

#include <stddef.h>
#include <stdint.h>

static void test_known_sums(void) {
    const int64_t matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6},
    };
    const int64_t negatives[3][2] = {
        {-2, 5},
        {0, -7},
        {4, 1},
    };
    const int64_t one[] = {42};
    const int64_t row[] = {1, 2, 3, 4, 5};

    EXPECT_LL(locality_sum_row_major(&matrix[0][0], 2, 3), 21);
    EXPECT_LL(locality_sum_column_major(&matrix[0][0], 2, 3), 21);
    EXPECT_LL(locality_sum_row_major(&negatives[0][0], 3, 2), 1);
    EXPECT_LL(locality_sum_column_major(&negatives[0][0], 3, 2), 1);
    EXPECT_LL(locality_sum_row_major(one, 1, 1), 42);
    EXPECT_LL(locality_sum_column_major(one, 1, 1), 42);
    EXPECT_LL(locality_sum_row_major(row, 1, 5), 15);
    EXPECT_LL(locality_sum_column_major(row, 5, 1), 15);
    EXPECT_LL(locality_sum_row_major(NULL, 0, 4), 0);
    EXPECT_LL(locality_sum_column_major(NULL, 3, 0), 0);
}

static void test_measure_and_arguments(void) {
    const int64_t matrix[4][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16},
    };
    int64_t sum = -1;
    double seconds = -1.0;

    EXPECT(locality_measure_row_major(&matrix[0][0], 4, 4, &sum, &seconds) == 0);
    EXPECT_LL(sum, 136);
    EXPECT(seconds >= 0.0);

    sum = -1;
    seconds = -1.0;
    EXPECT(locality_measure_column_major(&matrix[0][0], 4, 4, &sum, &seconds) == 0);
    EXPECT_LL(sum, 136);
    EXPECT(seconds >= 0.0);

    EXPECT(locality_measure_row_major(NULL, 2, 2, &sum, &seconds) == -1);
    EXPECT(locality_measure_column_major(NULL, 2, 2, &sum, &seconds) == -1);
    EXPECT(locality_measure_row_major(matrix[0], 4, 4, NULL, &seconds) == -1);
    EXPECT(locality_measure_column_major(matrix[0], 4, 4, &sum, NULL) == -1);
    EXPECT(locality_sum_row_major(NULL, 2, 2) == 0);
    EXPECT(locality_measure_row_major(NULL, SIZE_MAX, 2, &sum, &seconds) == -1);
    EXPECT(locality_measure_column_major(NULL, 0, 0, &sum, &seconds) == 0);
    EXPECT_LL(sum, 0);
    EXPECT(seconds >= 0.0);
}

int main(void) {
    test_known_sums();
    test_measure_and_arguments();
    return test_finish();
}
