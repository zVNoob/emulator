#pragma once

#include "cpu.hpp"
#include <cstdint>
#include <sched.h>

namespace cpu {
class CPU_64I : public CPU_Feature {
  inline int64_t sign_extend(uint64_t imm, uint8_t size) {
    return (int64_t)(imm << (64 - size)) >> (64 - size);
  }
  inline bool execute_I_arith(CPU_State &state, uint8_t op, uint8_t rd, uint8_t rs1, int32_t imm, uint8_t funct3) {
    switch (funct3) {
      case 0b000: // addw
      state.regs[rd] = (int32_t)(*(int64_t*)(&state.regs[rs1]) + imm);
      return true;
      case 0b001: // sllw
      state.regs[rd] = (int32_t)(state.regs[rs1] << imm);
      return true;
      case 0b101: // srlw & sraw
      if ((imm >> 11) == 0) {
        imm = (imm << 2) >> 2;
        state.regs[rd] = (int32_t)(state.regs[rs1] >> imm);
        return true;
      } else {
        imm = (imm << 2) >> 2;
        state.regs[rd] = (int32_t)(int64_t(state.regs[rs1]) >> imm);
        return true;
      }
      default:return false;
    }
  }
  inline bool execute_I_load(CPU_State &state, uint8_t op, uint8_t rd, uint8_t rs1, int32_t imm, uint8_t funct3) {
    switch (funct3) {
      case 0b011: // ld
      state.regs[rd] = state.mem->read(state.regs[rs1] + imm, 8);
      return true;
      case 0b110: // lwu
      state.regs[rd] = sign_extend(state.mem->read(state.regs[rs1] + imm, 4), 32);
      return true;
      default: return false;
    }
  }
  inline bool execute_S(CPU_State &state, uint8_t op, uint8_t rs1, uint8_t rs2, uint32_t full_opcode, uint8_t funct3) {
    int32_t imm = 0;
    {
      uint32_t imm_temp = 0;
      imm_temp |= get_segment(full_opcode, 7, 11);
      imm_temp |= get_segment(full_opcode, 25, 31) << 5;
      imm = sign_extend(imm_temp, 12);
    }
    switch (funct3) {
      case 0b011: // sd
      state.mem->write(state.regs[rs1] + imm, state.regs[rs2], 8);
      return true;
      default: return false;
    }
  }
  inline bool execute_R(CPU_State &state, uint32_t op, uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t funct3, uint8_t funct7) {
    switch (funct3) {
      case 0b000: // addw & subw
      if (funct7 == 0b0000000) { // addw
        state.regs[rd] = (int32_t)(*(int64_t*)(&state.regs[rs1]) + *(int64_t*)(&state.regs[rs2]));
        return true;
      } else if (funct7 == 0b0100000) { // subw
        state.regs[rd] = (int32_t)(*(int64_t*)(&state.regs[rs1]) - *(int64_t*)(&state.regs[rs2]));
        return true;
      }
      return false;
      case 0b001: // sllw
      state.regs[rd] = (int32_t)(state.regs[rs1] << state.regs[rs2]);
      return true;
      case 0b101: // srlw & sraw
      if ((funct7 >> 11) == 0) {
        state.regs[rd] = (int32_t)(state.regs[rs1] >> state.regs[rs2]);
        return true;
      } else {
        state.regs[rd] = (int32_t)(int64_t(state.regs[rs1]) >> state.regs[rs2]);
        return true;
      }
      default: return false;
    }
  }

public:
  bool execute(uint32_t opcode,CPU& state) override {
    switch (get_segment(opcode, 0, 6)) {
      case 0b0011011: // I-type arithmetic
      return execute_I_arith(state.state, get_segment(opcode, 0, 6),
                             get_segment(opcode, 7, 11),
                             get_segment(opcode, 15, 19),
                             sign_extend(get_segment(opcode, 20, 31), 12),
                             get_segment(opcode, 12, 14));
      case 0b0000011: // I-type load
      return execute_I_load(state.state, get_segment(opcode, 0, 6),
                            get_segment(opcode, 7, 11),
                            get_segment(opcode, 15, 19),
                            sign_extend(get_segment(opcode, 20, 31), 12),
                            get_segment(opcode, 12, 14));
      case 0b0100011: // S-type
      return execute_S(state.state, get_segment(opcode, 0, 6),
                       get_segment(opcode, 15, 19),
                       get_segment(opcode, 20, 24),
                       opcode,
                       get_segment(opcode, 12, 14));
      case 0b0110011: // R-type
      return execute_R(state.state, get_segment(opcode, 0, 6),
                       get_segment(opcode, 7, 11),
                       get_segment(opcode, 15, 19),
                       get_segment(opcode, 20, 24),
                       get_segment(opcode, 12, 14),
                       get_segment(opcode, 25, 31));
      default: return false;
    }
  }
};
}
