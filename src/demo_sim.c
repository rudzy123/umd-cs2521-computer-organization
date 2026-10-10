#include "sim.h"

#include <stdio.h>
#include <stdint.h>

/*
 * Sum 5+4+3+2+1 by counting down. r0 is the sum, r1 is the counter, r2 is 1.
 * This is the teaching CPU from include/sim.h, not an x86 program.
 * sim_step fetches the instruction at pc, decodes it, then executes it.
 */

static const Insn k_program[] = {
    {OPC_LDI, 0, 0, 0},
    {OPC_LDI, 1, 0, 5},
    {OPC_LDI, 2, 0, 1},
    {OPC_ADD, 0, 1, 0},
    {OPC_SUB, 1, 2, 0},
    {OPC_JZ, 1, 0, 7},
    {OPC_JMP, 0, 0, 3},
    {OPC_STORE, 0, 0, 0},
    {OPC_HALT, 0, 0, 0},
};

static void print_instruction(const Insn *insn) {
    printf("%-5s", sim_opcode_name(insn->op));
    switch (insn->op) {
    case OPC_LDI:
        printf(" r%u, %d", insn->a, (int)insn->imm);
        break;
    case OPC_ADD:
    case OPC_SUB:
        printf(" r%u, r%u", insn->a, insn->b);
        break;
    case OPC_LOAD:
    case OPC_STORE:
        printf(" r%u, mem[%d]", insn->a, (int)insn->imm);
        break;
    case OPC_JMP:
        printf(" %d", (int)insn->imm);
        break;
    case OPC_JZ:
        printf(" r%u, %d", insn->a, (int)insn->imm);
        break;
    default:
        break;
    }
}

int main(void) {
    Cpu cpu;
    unsigned guard;

    puts("Teaching CPU: fetch, decode, execute, halt");
    puts("The program counter is an index into a fixed-size instruction array.");
    puts("Data memory is a separate 256-word array. This is not the x86 ISA.");
    puts("");

    if (cpu_load_program(&cpu, k_program, sizeof k_program / sizeof k_program[0]) != 0) {
        fputs("could not load the sample program\n", stderr);
        return 1;
    }

    for (guard = 0; guard < 64u; ++guard) {
        uint32_t fetched_pc = cpu.pc;
        SimStatus status = sim_step(&cpu);
        if (status == SIM_FAULT && cpu.cycles == 0u) {
            printf("fetch fault: %s\n", sim_fault_name(cpu.fault_reason));
            return 1;
        }
        if (cpu.cycles == 0u) {
            fputs("simulator did not fetch\n", stderr);
            return 1;
        }
        printf("cycle %llu  fetch pc=%u  ", (unsigned long long)cpu.cycles, fetched_pc);
        print_instruction(&cpu.ir);
        printf("  execute -> pc=%u r0=%d r1=%d r2=%d mem[0]=%d\n", cpu.pc, cpu.regs[0], cpu.regs[1],
               cpu.regs[2], cpu.mem[0]);
        if (status == SIM_HALTED) {
            break;
        }
        if (status == SIM_FAULT) {
            printf("fault: %s\n", sim_fault_name(cpu.fault_reason));
            return 1;
        }
    }

    if (!cpu.halted || cpu.regs[0] != 15 || cpu.mem[0] != 15) {
        fputs("sample program did not compute 15\n", stderr);
        return 1;
    }
    puts("");
    puts("r0 and mem[0] hold 15, the sum of 1 through 5. The CPU is halted.");
    return 0;
}
