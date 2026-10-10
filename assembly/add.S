/*
 * int64_t asm_add_i64(int64_t left, int64_t right)
 *
 * System V AMD64 places left in %rdi and right in %rsi.
 * The sum returns in %rax. lea adds the registers without a separate add
 * instruction. The 64-bit result wraps modulo 2^64, matching ref_add_i64.
 */

    .text
    .globl asm_add_i64
    .type asm_add_i64, @function
asm_add_i64:
    leaq (%rdi, %rsi, 1), %rax
    ret
    .size asm_add_i64, .-asm_add_i64

    /* Tell the linker this object does not need an executable stack. */
    .section .note.GNU-stack,"",@progbits
