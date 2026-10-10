#ifndef CS2521_SIM_H
#define CS2521_SIM_H

#include <stddef.h>
#include <stdint.h>

/*
 * A teaching CPU with a fixed-size instruction and a separate program store.
 * The program counter is an index into that store, not an x86 byte address.
 * Data memory is a distinct array of 32-bit words. See docs/cpu-cycle.md.
 *
 * Immediates are signed 16-bit values (-32768..32767).
 * Register names are r0..r7. r0 is a normal register, not a hardwired zero.
 * Arithmetic wraps modulo 2^32 using unsigned arithmetic inside the simulator.
 */

#define SIM_REG_COUNT 8
#define SIM_MEM_WORDS 256
#define SIM_MAX_PROGRAM 128

typedef enum {
    OPC_HALT = 0,
    OPC_LDI = 1,
    OPC_ADD = 2,
    OPC_SUB = 3,
    OPC_LOAD = 4,
    OPC_STORE = 5,
    OPC_JMP = 6,
    OPC_JZ = 7
} Opcode;

#define SIM_OPC_COUNT 8

typedef enum {
    FAULT_NONE = 0,
    FAULT_PC = 1,
    FAULT_OPCODE = 2,
    FAULT_REGISTER = 3,
    FAULT_MEMORY = 4
} FaultReason;

typedef enum {
    SIM_OK = 0,
    SIM_HALTED = 1,
    SIM_FAULT = -1
} SimStatus;

typedef struct {
    uint8_t op;
    uint8_t a;
    uint8_t b;
    int16_t imm;
} Insn;

typedef struct {
    uint8_t opcode;
    uint8_t reg_a;
    uint8_t reg_b;
    int32_t immediate;
} DecodedInsn;

typedef struct {
    uint32_t pc;
    uint32_t program_length;
    uint64_t cycles;
    int halted;
    int fault;
    FaultReason fault_reason;
    int32_t regs[SIM_REG_COUNT];
    int32_t mem[SIM_MEM_WORDS];
    Insn program[SIM_MAX_PROGRAM];
    Insn ir;
} Cpu;

void cpu_reset(Cpu *cpu);

/* Returns 0 on success. A rejected load leaves the previous Cpu contents in place. */
int cpu_load_program(Cpu *cpu, const Insn *program, size_t length);

SimStatus sim_fetch(Cpu *cpu);
SimStatus sim_decode(const Cpu *cpu, DecodedInsn *decoded);
SimStatus sim_execute(Cpu *cpu, const DecodedInsn *decoded);
SimStatus sim_step(Cpu *cpu);

/*
 * Runs until halt, fault, or max_steps successful SIM_OK results.
 * A step budget that expires returns SIM_OK with halted still clear.
 * max_steps of 0 does not execute anything and returns SIM_OK.
 */
SimStatus sim_run(Cpu *cpu, uint32_t max_steps);

const char *sim_opcode_name(uint8_t opcode);
const char *sim_fault_name(FaultReason reason);

#endif
