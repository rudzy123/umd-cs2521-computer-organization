/*
 * int64_t asm_sum_i64(const int64_t *values, size_t count)
 *
 * %rdi is the address of the first element and %rsi is the count.
 * An empty count returns 0 and does not load from the pointer.
 * addq wraps modulo 2^64, the same wrap ref_sum_i64 builds with uint64_t.
 */

    .text
    .globl asm_sum_i64
    .type asm_sum_i64, @function
asm_sum_i64:
    xorq %rax, %rax
    xorq %rcx, %rcx
.Lsum_loop:
    cmpq %rsi, %rcx
    jae .Lsum_done
    addq (%rdi, %rcx, 8), %rax
    incq %rcx
    jmp .Lsum_loop
.Lsum_done:
    ret
    .size asm_sum_i64, .-asm_sum_i64

    .section .note.GNU-stack,"",@progbits
