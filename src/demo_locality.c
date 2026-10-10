#include "locality.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

enum { DEMO_N = 768, DEMO_REPEATS = 3 };

static void report(const char *name, int repeat, int64_t sum, double seconds) {
    printf("run %d  %-13s %8.6f s  sum %lld\n", repeat, name, seconds, (long long)sum);
}

int main(void) {
    const size_t cells = (size_t)DEMO_N * (size_t)DEMO_N;
    int64_t *matrix;
    int64_t expected = 0;
    double row_total = 0.0;
    double column_total = 0.0;
    int repeat;
    size_t index;

    puts("Cache locality demonstration");
    puts("WARNING: Elapsed time varies with this machine, its cache sizes, the");
    puts("compiler optimization level, and other programs running right now.");
    puts("Record the numbers you observe. Do not expect a fixed speed ratio.");
    puts("");

    matrix = calloc(cells, sizeof *matrix);
    if (matrix == NULL) {
        fputs("could not allocate the matrix\n", stderr);
        return 1;
    }
    for (index = 0; index < cells; ++index) {
        matrix[index] = (int64_t)((int)(index % 17u) - 8);
        expected += matrix[index];
    }

    printf("matrix: %d x %d int64 values (%zu bytes), repeats: %d\n", DEMO_N, DEMO_N,
           cells * sizeof *matrix, DEMO_REPEATS);
    printf("A common x86-64 cache line is 64 bytes, which holds %zu of these values.\n",
           (size_t)64 / sizeof(int64_t));
    printf("Column-major stride: %zu bytes.\n", (size_t)DEMO_N * sizeof(int64_t));
    puts("Row-major reads consecutive addresses. Column-major jumps by one row.");
    puts("");

    for (repeat = 1; repeat <= DEMO_REPEATS; ++repeat) {
        int64_t row_sum = 0;
        int64_t column_sum = 0;
        double row_seconds = 0.0;
        double column_seconds = 0.0;
        if (locality_measure_row_major(matrix, DEMO_N, DEMO_N, &row_sum, &row_seconds) != 0 ||
            locality_measure_column_major(matrix, DEMO_N, DEMO_N, &column_sum, &column_seconds) !=
                0) {
            fputs("timing call failed\n", stderr);
            free(matrix);
            return 1;
        }
        report("row-major", repeat, row_sum, row_seconds);
        report("column-major", repeat, column_sum, column_seconds);
        if (row_sum != expected || column_sum != expected) {
            fputs("traversals disagree with the checksum\n", stderr);
            free(matrix);
            return 1;
        }
        row_total += row_seconds;
        column_total += column_seconds;
    }

    printf("\nrow-major mean:     %8.6f s\n", row_total / DEMO_REPEATS);
    printf("column-major mean:  %8.6f s\n", column_total / DEMO_REPEATS);
    puts("Both traversals produced the same integer sum. The access order is what changes.");
    free(matrix);
    return 0;
}
