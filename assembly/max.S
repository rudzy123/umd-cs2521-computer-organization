/*
 * int64_t asm_max_i64(const int64_t *values, size_t count)
 *
 * Precondition: values is non-null and count is at least 1.
 * cmpq subtracts for the flags only. cmovg then keeps the signed greater value.
 * %rcx is caller-saved, so this leaf function may use it without saving it.
 */

    .text
    .globl asm_max_i64
    .type asm_max_i64, @function
asm_max_i64:
    movq (%rdi), %rax
    cmpq $1, %rsi
    jbe .Lmax_done
    movq $1, %rcx
.Lmax_loop:
    movq (%rdi, %rcx, 8), %rdx
    cmpq %rax, %rdx
    cmovg %rdx, %rax
    incq %rcx
    cmpq %rsi, %rcx
    jb .Lmax_loop
.Lmax_done:
    ret
    .size asm_max_i64, .-asm_max_i64

    .section .note.GNU-stack,"",@progbits
