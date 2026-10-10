#include "sim.h"

#include <string.h>

static int32_t wrap_from_u32(uint32_t bits) {
    int32_t value = 0;
    memcpy(&value, &bits, sizeof value);
    return value;
}

void cpu_reset(Cpu *cpu) {
    if (cpu == NULL) {
        return;
    }
    memset(cpu, 0, sizeof *cpu);
}

int cpu_load_program(Cpu *cpu, const Insn *program, size_t length) {
    size_t index;
    if (cpu == NULL || length > (size_t)SIM_MAX_PROGRAM || (program == NULL && length != 0u)) {
        return -1;
    }
    cpu_reset(cpu);
    for (index = 0; index < length; ++index) {
        cpu->program[index] = program[index];
    }
    cpu->program_length = (uint32_t)length;
    return 0;
}

const char *sim_opcode_name(uint8_t opcode) {
    switch (opcode) {
    case OPC_HALT:
        return "HALT";
    case OPC_LDI:
        return "LDI";
    case OPC_ADD:
        return "ADD";
    case OPC_SUB:
        return "SUB";
    case OPC_LOAD:
        return "LOAD";
    case OPC_STORE:
        return "STORE";
    case OPC_JMP:
        return "JMP";
    case OPC_JZ:
        return "JZ";
    default:
        return "UNKNOWN";
    }
}

const char *sim_fault_name(FaultReason reason) {
    switch (reason) {
    case FAULT_NONE:
        return "none";
    case FAULT_PC:
        return "program counter";
    case FAULT_OPCODE:
        return "opcode";
    case FAULT_REGISTER:
        return "register";
    case FAULT_MEMORY:
        return "memory";
    default:
        return "unknown";
    }
}

static SimStatus fail(Cpu *cpu, FaultReason reason) {
    cpu->fault = 1;
    cpu->fault_reason = reason;
    return SIM_FAULT;
}

SimStatus sim_fetch(Cpu *cpu) {
    if (cpu == NULL) {
        return SIM_FAULT;
    }
    if (cpu->halted) {
        return SIM_HALTED;
    }
    if (cpu->fault) {
        return SIM_FAULT;
    }
    if (cpu->pc >= cpu->program_length) {
        return fail(cpu, FAULT_PC);
    }
    cpu->ir = cpu->program[cpu->pc];
    return SIM_OK;
}

static int register_in_range(uint8_t index) {
    return index < SIM_REG_COUNT;
}

SimStatus sim_decode(const Cpu *cpu, DecodedInsn *decoded) {
    if (cpu == NULL || decoded == NULL) {
        return SIM_FAULT;
    }
    memset(decoded, 0, sizeof *decoded);
    decoded->opcode = cpu->ir.op;
    decoded->reg_a = cpu->ir.a;
    decoded->reg_b = cpu->ir.b;
    decoded->immediate = cpu->ir.imm;
    switch (cpu->ir.op) {
    case OPC_HALT:
    case OPC_JMP:
        return SIM_OK;
    case OPC_LDI:
    case OPC_LOAD:
    case OPC_STORE:
    case OPC_JZ:
        if (!register_in_range(cpu->ir.a)) {
            return SIM_FAULT;
        }
        return SIM_OK;
    case OPC_ADD:
    case OPC_SUB:
        if (!register_in_range(cpu->ir.a) || !register_in_range(cpu->ir.b)) {
            return SIM_FAULT;
        }
        return SIM_OK;
    default:
        return SIM_FAULT;
    }
}

static SimStatus need_reg(Cpu *cpu, uint8_t index) {
    if (index >= SIM_REG_COUNT) {
        return fail(cpu, FAULT_REGISTER);
    }
    return SIM_OK;
}

static SimStatus jump_to(Cpu *cpu, int32_t target) {
    if (target < 0 || (uint32_t)target >= cpu->program_length) {
        return fail(cpu, FAULT_PC);
    }
    cpu->pc = (uint32_t)target;
    return SIM_OK;
}

static SimStatus advance(Cpu *cpu) {
    cpu->pc += 1u;
    return SIM_OK;
}

SimStatus sim_execute(Cpu *cpu, const DecodedInsn *decoded) {
    int32_t address;
    if (cpu == NULL || decoded == NULL) {
        return SIM_FAULT;
    }
    if (cpu->halted) {
        return SIM_HALTED;
    }
    if (cpu->fault) {
        return SIM_FAULT;
    }
    switch (decoded->opcode) {
    case OPC_HALT:
        cpu->halted = 1;
        return SIM_HALTED;
    case OPC_LDI:
        if (need_reg(cpu, decoded->reg_a) != SIM_OK) {
            return SIM_FAULT;
        }
        cpu->regs[decoded->reg_a] = decoded->immediate;
        return advance(cpu);
    case OPC_ADD:
        if (need_reg(cpu, decoded->reg_a) != SIM_OK || need_reg(cpu, decoded->reg_b) != SIM_OK) {
            return SIM_FAULT;
        }
        cpu->regs[decoded->reg_a] = wrap_from_u32((uint32_t)cpu->regs[decoded->reg_a] +
                                                  (uint32_t)cpu->regs[decoded->reg_b]);
        return advance(cpu);
    case OPC_SUB:
        if (need_reg(cpu, decoded->reg_a) != SIM_OK || need_reg(cpu, decoded->reg_b) != SIM_OK) {
            return SIM_FAULT;
        }
        cpu->regs[decoded->reg_a] = wrap_from_u32((uint32_t)cpu->regs[decoded->reg_a] -
                                                  (uint32_t)cpu->regs[decoded->reg_b]);
        return advance(cpu);
    case OPC_LOAD:
        if (need_reg(cpu, decoded->reg_a) != SIM_OK) {
            return SIM_FAULT;
        }
        address = decoded->immediate;
        if (address < 0 || address >= SIM_MEM_WORDS) {
            return fail(cpu, FAULT_MEMORY);
        }
        cpu->regs[decoded->reg_a] = cpu->mem[address];
        return advance(cpu);
    case OPC_STORE:
        if (need_reg(cpu, decoded->reg_a) != SIM_OK) {
            return SIM_FAULT;
        }
        address = decoded->immediate;
        if (address < 0 || address >= SIM_MEM_WORDS) {
            return fail(cpu, FAULT_MEMORY);
        }
        cpu->mem[address] = cpu->regs[decoded->reg_a];
        return advance(cpu);
    case OPC_JMP:
        return jump_to(cpu, decoded->immediate);
    case OPC_JZ:
        if (need_reg(cpu, decoded->reg_a) != SIM_OK) {
            return SIM_FAULT;
        }
        if (cpu->regs[decoded->reg_a] == 0) {
            return jump_to(cpu, decoded->immediate);
        }
        return advance(cpu);
    default:
        return fail(cpu, FAULT_OPCODE);
    }
}

SimStatus sim_step(Cpu *cpu) {
    DecodedInsn decoded;
    SimStatus status;
    if (cpu == NULL) {
        return SIM_FAULT;
    }
    if (cpu->halted) {
        return SIM_HALTED;
    }
    if (cpu->fault) {
        return SIM_FAULT;
    }
    status = sim_fetch(cpu);
    if (status != SIM_OK) {
        return status;
    }
    cpu->cycles += 1u;
    status = sim_decode(cpu, &decoded);
    if (status != SIM_OK) {
        if (decoded.opcode >= SIM_OPC_COUNT) {
            return fail(cpu, FAULT_OPCODE);
        }
        return fail(cpu, FAULT_REGISTER);
    }
    return sim_execute(cpu, &decoded);
}

SimStatus sim_run(Cpu *cpu, uint32_t max_steps) {
    uint32_t step;
    SimStatus status = SIM_OK;
    if (cpu == NULL) {
        return SIM_FAULT;
    }
    for (step = 0; step < max_steps; ++step) {
        status = sim_step(cpu);
        if (status != SIM_OK) {
            return status;
        }
    }
    return status;
}
