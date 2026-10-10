/*
 * uint64_t asm_factorial_u64(uint64_t n, int *overflow)
 *
 * %rdi holds n and %rsi holds the overflow flag. The product returns in %rax.
 * 0! and 1! are 1. The loop multiplies by 2, 3, ... and refuses a factor that
 * would make the product not fit in 64 bits. 20! fits. 21! does not.
 *
 * The prologue is a normal stack frame. call already pushed the return address.
 * push %rbp saves the caller's frame pointer, and mov %rsp, %rbp names this
 * frame. %rbx is callee-saved, so it has to be restored before ret.
 */

    .text
    .globl asm_factorial_u64
    .type asm_factorial_u64, @function
asm_factorial_u64:
    pushq %rbp
    movq %rsp, %rbp
    pushq %rbx

    movl $0, (%rsi)
    movq $1, %rax
    cmpq $1, %rdi
    jbe .Lfactorial_done

    movq $2, %rcx
.Lfactorial_loop:
    movq %rax, %rbx
    movq $-1, %rax
    xorq %rdx, %rdx
    divq %rcx
    cmpq %rax, %rbx
    ja .Lfactorial_overflow
    movq %rbx, %rax
    mulq %rcx
    incq %rcx
    cmpq %rdi, %rcx
    jbe .Lfactorial_loop

.Lfactorial_done:
    popq %rbx
    popq %rbp
    ret

.Lfactorial_overflow:
    movl $1, (%rsi)
    xorq %rax, %rax
    jmp .Lfactorial_done

    .size asm_factorial_u64, .-asm_factorial_u64
    .section .note.GNU-stack,"",@progbits
