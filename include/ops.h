#ifndef CS2521_OPS_H
#define CS2521_OPS_H

#include <stddef.h>
#include <stdint.h>

/*
 * Assembly routines in the assembly directory and matching C reference routines.
 *
 * asm_add_i64 and the sum routines wrap modulo 2^64, the same way a 64-bit
 * adder does. The C references reproduce that wrap with unsigned arithmetic.
 * They do not perform a signed C addition that could overflow.
 *
 * asm_max_i64 / ref_max_i64 require a non-null array and count >= 1.
 * An empty sum is defined as 0. A null sum pointer with count 0 is defined as 0.
 * asm_factorial_u64 requires a non-null overflow flag.
 * 0! and 1! are 1. 20! fits in uint64_t. 21! and above set *overflow to 1
 * and return 0. On success *overflow is 0.
 */

int64_t asm_add_i64(int64_t left, int64_t right);
int64_t asm_sum_i64(const int64_t *values, size_t count);
int64_t asm_max_i64(const int64_t *values, size_t count);
uint64_t asm_factorial_u64(uint64_t n, int *overflow);

int64_t ref_add_i64(int64_t left, int64_t right);
int64_t ref_sum_i64(const int64_t *values, size_t count);
int64_t ref_max_i64(const int64_t *values, size_t count);
uint64_t ref_factorial_u64(uint64_t n, int *overflow);

#endif
