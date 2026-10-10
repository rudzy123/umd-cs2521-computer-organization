#include "sim.h"
#include "test_harness.h"

#include <stdint.h>
#include <string.h>

static int same_architecture(const Cpu *left, const Cpu *right) {
    return left->pc == right->pc && left->halted == right->halted && left->fault == right->fault &&
           left->fault_reason == right->fault_reason &&
           memcmp(left->regs, right->regs, sizeof left->regs) == 0 &&
           memcmp(left->mem, right->mem, sizeof left->mem) == 0;
}

static void run_phases(Cpu *cpu) {
    for (;;) {
        DecodedInsn decoded;
        SimStatus status = sim_fetch(cpu);
        if (status != SIM_OK) {
            break;
        }
        status = sim_decode(cpu, &decoded);
        if (status != SIM_OK) {
            cpu->fault = 1;
            cpu->fault_reason = decoded.opcode >= SIM_OPC_COUNT ? FAULT_OPCODE : FAULT_REGISTER;
            break;
        }
        status = sim_execute(cpu, &decoded);
        if (status != SIM_OK) {
            break;
        }
    }
}

static void test_arithmetic_and_memory(void) {
    Cpu cpu;
    static const Insn k_double[] = {
        {OPC_LDI, 0, 0, 21},
        {OPC_ADD, 0, 0, 0},
        {OPC_HALT, 0, 0, 0},
    };
    static const Insn k_sub[] = {
        {OPC_LDI, 0, 0, 10},
        {OPC_LDI, 1, 0, 3},
        {OPC_SUB, 0, 1, 0},
        {OPC_HALT, 0, 0, 0},
    };
    static const Insn k_negative[] = {
        {OPC_LDI, 0, 0, 0},
        {OPC_LDI, 1, 0, 1},
        {OPC_SUB, 0, 1, 0},
        {OPC_HALT, 0, 0, 0},
    };
    static const Insn k_memory[] = {
        {OPC_LDI, 0, 9, 42},
        {OPC_STORE, 0, 0, 10},
        {OPC_LDI, 0, 0, 0},
        {OPC_LOAD, 1, 0, 10},
        {OPC_HALT, 0, 0, 0},
    };
    static const Insn k_wrap[] = {
        {OPC_LOAD, 0, 0, 0},
        {OPC_LOAD, 1, 0, 1},
        {OPC_ADD, 0, 1, 0},
        {OPC_HALT, 0, 0, 0},
    };

    EXPECT(cpu_load_program(&cpu, k_double, 3) == 0);
    EXPECT(sim_run(&cpu, 8) == SIM_HALTED);
    EXPECT_LL(cpu.regs[0], 42);

    EXPECT(cpu_load_program(&cpu, k_sub, 4) == 0);
    EXPECT(sim_run(&cpu, 8) == SIM_HALTED);
    EXPECT_LL(cpu.regs[0], 7);

    EXPECT(cpu_load_program(&cpu, k_negative, 4) == 0);
    EXPECT(sim_run(&cpu, 8) == SIM_HALTED);
    EXPECT_LL(cpu.regs[0], -1);

    EXPECT(cpu_load_program(&cpu, k_memory, 5) == 0);
    EXPECT(sim_run(&cpu, 8) == SIM_HALTED);
    EXPECT_LL(cpu.regs[0], 0);
    EXPECT_LL(cpu.regs[1], 42);
    EXPECT_LL(cpu.mem[10], 42);
    EXPECT_LL(cpu.mem[9], 0);
    EXPECT_LL(cpu.mem[11], 0);

    EXPECT(cpu_load_program(&cpu, k_wrap, 4) == 0);
    cpu.mem[0] = INT32_MAX;
    cpu.mem[1] = 1;
    EXPECT(sim_run(&cpu, 8) == SIM_HALTED);
    EXPECT_LL(cpu.regs[0], INT32_MIN);
}

static void test_control(void) {
    Cpu cpu;
    static const Insn k_sum[] = {
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
    static const Insn k_jz_skip[] = {
        {OPC_LDI, 0, 0, 1},
        {OPC_JZ, 0, 0, 4},
        {OPC_LDI, 0, 0, 9},
        {OPC_HALT, 0, 0, 0},
        {OPC_LDI, 1, 0, 4},
        {OPC_HALT, 0, 0, 0},
    };
    static const Insn k_jz_take[] = {
        {OPC_LDI, 0, 0, 0},
        {OPC_JZ, 0, 0, 4},
        {OPC_LDI, 0, 0, 9},
        {OPC_HALT, 0, 0, 0},
        {OPC_LDI, 1, 0, 4},
        {OPC_HALT, 0, 0, 0},
    };
    static const Insn k_spin[] = {
        {OPC_JMP, 9, 9, 0},
    };
    uint64_t cycles;

    EXPECT(cpu_load_program(&cpu, k_sum, 9) == 0);
    EXPECT(sim_run(&cpu, 64) == SIM_HALTED);
    EXPECT_LL(cpu.regs[0], 15);
    EXPECT_LL(cpu.mem[0], 15);
    EXPECT(cpu.halted == 1);
    cycles = cpu.cycles;
    EXPECT(sim_step(&cpu) == SIM_HALTED);
    EXPECT(cpu.cycles == cycles);

    EXPECT(cpu_load_program(&cpu, k_jz_skip, 6) == 0);
    EXPECT(sim_run(&cpu, 8) == SIM_HALTED);
    EXPECT_LL(cpu.regs[0], 9);
    EXPECT_LL(cpu.regs[1], 0);

    EXPECT(cpu_load_program(&cpu, k_jz_take, 6) == 0);
    EXPECT(sim_run(&cpu, 8) == SIM_HALTED);
    EXPECT_LL(cpu.regs[0], 0);
    EXPECT_LL(cpu.regs[1], 4);

    EXPECT(cpu_load_program(&cpu, k_spin, 1) == 0);
    EXPECT(sim_run(&cpu, 5) == SIM_OK);
    EXPECT(cpu.halted == 0);
    EXPECT(cpu.fault == 0);
    EXPECT(cpu.pc == 0u);
    EXPECT(cpu.cycles == 5u);
}

static void test_faults_and_limits(void) {
    Cpu cpu;
    Insn big[SIM_MAX_PROGRAM + 1];
    Insn full[SIM_MAX_PROGRAM];
    static const Insn k_one[] = {
        {OPC_LDI, 0, 0, 7},
    };
    static const Insn k_halt[] = {
        {OPC_HALT, 9, 9, 50},
    };
    static const Insn k_bad_op[] = {
        {99, 0, 0, 0},
    };
    static const Insn k_bad_reg[] = {
        {OPC_LDI, 8, 0, 1},
    };
    static const Insn k_bad_mem[] = {
        {OPC_LDI, 0, 0, 3},
        {OPC_STORE, 0, 0, -1},
    };
    static const Insn k_bad_mem_high[] = {
        {OPC_STORE, 0, 0, 256},
    };
    static const Insn k_bad_jump[] = {
        {OPC_JMP, 0, 0, -1},
    };
    size_t index;

    EXPECT(cpu_load_program(NULL, k_one, 1) == -1);
    EXPECT(sim_step(NULL) == SIM_FAULT);
    EXPECT(sim_run(NULL, 1) == SIM_FAULT);
    cpu_reset(NULL);

    EXPECT(cpu_load_program(&cpu, NULL, 0) == 0);
    EXPECT(sim_step(&cpu) == SIM_FAULT);
    EXPECT(cpu.fault_reason == FAULT_PC);
    EXPECT(cpu.cycles == 0u);

    EXPECT(cpu_load_program(&cpu, k_one, 1) == 0);
    EXPECT(sim_step(&cpu) == SIM_OK);
    EXPECT(cpu.pc == 1u);
    EXPECT_LL(cpu.regs[0], 7);
    EXPECT(sim_step(&cpu) == SIM_FAULT);
    EXPECT(cpu.fault_reason == FAULT_PC);
    EXPECT(cpu.cycles == 1u);

    EXPECT(cpu_load_program(&cpu, k_halt, 1) == 0);
    EXPECT(sim_step(&cpu) == SIM_HALTED);
    EXPECT(cpu.fault == 0);
    EXPECT(cpu.cycles == 1u);

    EXPECT(cpu_load_program(&cpu, k_bad_op, 1) == 0);
    EXPECT(sim_step(&cpu) == SIM_FAULT);
    EXPECT(cpu.fault_reason == FAULT_OPCODE);
    EXPECT(cpu.pc == 0u);
    EXPECT(cpu.cycles == 1u);
    EXPECT(sim_step(&cpu) == SIM_FAULT);
    EXPECT(cpu.cycles == 1u);

    EXPECT(cpu_load_program(&cpu, k_bad_reg, 1) == 0);
    EXPECT(sim_step(&cpu) == SIM_FAULT);
    EXPECT(cpu.fault_reason == FAULT_REGISTER);
    EXPECT(cpu.pc == 0u);

    EXPECT(cpu_load_program(&cpu, k_bad_mem, 2) == 0);
    EXPECT(sim_run(&cpu, 4) == SIM_FAULT);
    EXPECT(cpu.fault_reason == FAULT_MEMORY);
    EXPECT_LL(cpu.mem[0], 0);

    EXPECT(cpu_load_program(&cpu, k_bad_mem_high, 1) == 0);
    EXPECT(sim_step(&cpu) == SIM_FAULT);
    EXPECT(cpu.fault_reason == FAULT_MEMORY);

    EXPECT(cpu_load_program(&cpu, k_bad_jump, 1) == 0);
    EXPECT(sim_step(&cpu) == SIM_FAULT);
    EXPECT(cpu.fault_reason == FAULT_PC);
    EXPECT(cpu.pc == 0u);

    cpu_reset(&cpu);
    cpu.regs[0] = 5;
    for (index = 0; index < SIM_MAX_PROGRAM + 1u; ++index) {
        big[index].op = OPC_HALT;
        big[index].a = 0;
        big[index].b = 0;
        big[index].imm = 0;
    }
    EXPECT(cpu_load_program(&cpu, big, SIM_MAX_PROGRAM + 1u) == -1);
    EXPECT_LL(cpu.regs[0], 5);

    for (index = 0; index < SIM_MAX_PROGRAM; ++index) {
        full[index] = big[index];
    }
    EXPECT(cpu_load_program(&cpu, full, SIM_MAX_PROGRAM) == 0);
    EXPECT(cpu.program_length == SIM_MAX_PROGRAM);
    EXPECT(cpu_load_program(&cpu, NULL, 1) == -1);
    EXPECT(cpu.program_length == SIM_MAX_PROGRAM);
}

static void test_phases_match_step(void) {
    Cpu stepped;
    Cpu phased;
    static const Insn k_sum[] = {
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
    static const Insn k_bad_op[] = {
        {200, 1, 2, 3},
    };

    EXPECT(cpu_load_program(&stepped, k_sum, 9) == 0);
    EXPECT(cpu_load_program(&phased, k_sum, 9) == 0);
    EXPECT(sim_run(&stepped, 64) == SIM_HALTED);
    run_phases(&phased);
    EXPECT(same_architecture(&stepped, &phased));

    EXPECT(cpu_load_program(&stepped, k_bad_op, 1) == 0);
    EXPECT(cpu_load_program(&phased, k_bad_op, 1) == 0);
    EXPECT(sim_step(&stepped) == SIM_FAULT);
    run_phases(&phased);
    EXPECT(same_architecture(&stepped, &phased));
    EXPECT(stepped.fault_reason == FAULT_OPCODE);
}

static void test_names(void) {
    EXPECT(strcmp(sim_opcode_name(OPC_HALT), "HALT") == 0);
    EXPECT(strcmp(sim_opcode_name(OPC_ADD), "ADD") == 0);
    EXPECT(strcmp(sim_opcode_name(OPC_JZ), "JZ") == 0);
    EXPECT(strcmp(sim_opcode_name(99), "UNKNOWN") == 0);
    EXPECT(strcmp(sim_fault_name(FAULT_NONE), "none") == 0);
    EXPECT(strcmp(sim_fault_name(FAULT_MEMORY), "memory") == 0);
    EXPECT(strcmp(sim_fault_name(FAULT_REGISTER), "register") == 0);
}

int main(void) {
    test_arithmetic_and_memory();
    test_control();
    test_faults_and_limits();
    test_phases_match_step();
    test_names();
    return test_finish();
}
