#pragma once

#include "cpu.hpp"
#include <cstdint>

namespace cpu {
class CPU_I : public CPU_Feature {
  inline bool execute_R(CPU_State &state, uint32_t op, uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t funct3, uint8_t funct7) {
    switch (funct3) {
      case 0b000: // add & sub
      if (funct7 == 0b0000000) { // add
        state.regs[rd] = 
            state.regs[rs1] + 
            state.regs[rs2];
        return true;
      } else if (funct7 == 0b0100000) { // sub
        state.regs[rd] = 
            state.regs[rs1] - 
            state.regs[rs2];
        return true;
      }
      return false;
      case 0b001: // sll
      state.regs[rd] = 
          state.regs[rs1] <<
          state.regs[rs2];
      return true;
      case 0b010: // slt
      state.regs[rd] =
          (int64_t)state.regs[rs1] <
          (int64_t)state.regs[rs2];
      return true;
      case 0b011: // sltu
      state.regs[rd] =
          state.regs[rs1] <
          state.regs[rs2];
      return true;
      case 0b100: // xor
      state.regs[rd] =
          state.regs[rs1] ^
          state.regs[rs2];
      return true;
      case 0b101: // srl
      if (funct7 == 0b0000000) {
        state.regs[rd] =
            state.regs[rs1] >>
            state.regs[rs2];
      } else { // sra
        state.regs[rd] =
            (int64_t)state.regs[rs1] >>
            state.regs[rs2];
      }
      return true;
      case 0b110: // or
      state.regs[rd] =
          state.regs[rs1] |
          state.regs[rs2];
      return true;
      case 0b111: // and
      state.regs[rd] =
          state.regs[rs1] &
          state.regs[rs2];
      return true;
      default:
      return false;
    }
  }
  inline bool execute_I_arith(CPU_State &state, uint32_t op, uint8_t rd, uint8_t rs1, int32_t imm, uint8_t funct3) {
    switch (funct3) {
      case 0b000: // addi
      state.regs[rd] = 
          state.regs[rs1] + 
          imm;
      return true;
      case 0b010: // slti
      state.regs[rd] =
          (int64_t)state.regs[rs1] <
          imm;
      return true;
      case 0b011: // sltiu
      state.regs[rd] =
          state.regs[rs1] <
          imm;
      return true;
      case 0b100: // xori
      state.regs[rd] =
          state.regs[rs1] ^
          imm;
      return true;
      case 0b110: // ori
      state.regs[rd] =
          state.regs[rs1] |
          imm;
      return true;
      case 0b111: // andi
      state.regs[rd] =
          state.regs[rs1] &
          imm;
      return true;
      case 0b001: // slli
      state.regs[rd] =
          state.regs[rs1] <<
          imm;
      return true;
      case 0b101: // srli & srai
      if ((imm >> 11) == 0) {
        imm = (imm << 2) >> 2;
        state.regs[rd] =
            state.regs[rs1] >>
            imm;
      } else {
        imm = (imm << 2) >> 2;
        state.regs[rd] =
            (int64_t)state.regs[rs1] >>
            imm;
      }
      return true;
      default:return false;
    }
  }
  inline bool execute_I_load(CPU_State &state, uint8_t op, uint8_t rd, uint8_t rs1, int32_t imm, uint8_t funct3) {
    switch (funct3) {
      case 0b000: //lb
      state.regs[rd] = sign_extend(state.mem->read(state.regs[rs1] + imm, 1), 8);
      return true;
      case 0b001: //lh
      state.regs[rd] = sign_extend(state.mem->read(state.regs[rs1] + imm, 2), 16);
      return true;
      case 0b010: //lw
      state.regs[rd] = sign_extend(state.mem->read(state.regs[rs1] + imm, 4), 32);
      return true;
      case 0b100: //lbu
      state.regs[rd] = state.mem->read(state.regs[rs1] + imm, 1);
      return true;
      case 0b101: //lhu
      state.regs[rd] = state.mem->read(state.regs[rs1] + imm, 2);
      return true;
      default: return false;
    }
  }
  inline bool execute_U(CPU_State &state, uint8_t op, uint8_t rd, int32_t imm) {
    switch(op) {
      case 0b0110111: // lui
      state.regs[rd] = imm << 12;
      return true;
      case 0b0010111: // auipc
      state.regs[rd] = state.pc + (imm << 12);
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
      case 0b000: // sb
      state.mem->write(state.regs[rs1] + imm, state.regs[rs2] & 0xFF, 1);
      return true;
      case 0b001: // sh
      state.mem->write(state.regs[rs1] + imm, state.regs[rs2] & 0xFFFF, 2);
      return true;
      case 0b010: // sw
      state.mem->write(state.regs[rs1] + imm, state.regs[rs2], 4);
      return true;
      default: return false;
    }
  }
  inline bool execute_B(CPU_State &state, uint8_t op, uint8_t rs1, uint8_t rs2, uint8_t funct3, uint32_t full_opcode) {
    int32_t imm = 0;
    {
      uint32_t imm_temp = 0;
      imm_temp |= get_segment(full_opcode, 31, 31) << 12;
      imm_temp |= get_segment(full_opcode, 7, 7) << 11;
      imm_temp |= get_segment(full_opcode, 25, 30) << 5;
      imm_temp |= get_segment(full_opcode, 8, 11) << 1;
      imm = sign_extend(imm_temp, 13);
    }
    imm -= 4;
    switch (funct3) {
      case 0b000: // beq
      if (state.regs[rs1] == state.regs[rs2]) state.pc += imm;
      return true;
      case 0b001: // bne
      if (state.regs[rs1] != state.regs[rs2]) state.pc += imm;
      return true;
      case 0b100: // blt
      if ((int64_t)state.regs[rs1] < (int64_t)state.regs[rs2]) state.pc += imm;
      return true;
      case 0b101: // bge
      if ((int64_t)state.regs[rs1] >= (int64_t)state.regs[rs2]) state.pc += imm;
      return true;
      case 0b110: // bltu
      if (state.regs[rs1] < state.regs[rs2]) state.pc += imm;
      return true;
      case 0b111: // bgeu
      if (state.regs[rs1] >= state.regs[rs2]) state.pc += imm;
      return true;
      default:return false;
    }
  }
  inline bool execute_J(CPU_State &state, uint8_t op, uint8_t rd, uint32_t full_opcode) {
    int32_t imm = 0;
    {
      uint32_t imm_temp = 0;
      imm_temp |= get_segment(full_opcode, 31, 31) << 20;
      imm_temp |= get_segment(full_opcode, 21, 30) << 1;
      imm_temp |= get_segment(full_opcode, 20, 20) << 11;
      imm_temp |= get_segment(full_opcode, 12, 19) << 12;
      imm = sign_extend(imm_temp, 21);
    }
    state.regs[rd] = state.pc + 4;
    state.pc += imm - 4;
    return true;
  }
  inline int64_t sign_extend(uint64_t imm, uint8_t size) {
    return (int64_t)(imm << (64 - size)) >> (64 - size);
  }
  bool execute(uint32_t opcode, CPU& cpu) override {
    switch (get_segment(opcode, 0, 6)) {
      case 0b0110011: // R-type
      return execute_R(cpu.state, get_segment(opcode, 0, 6), 
                       get_segment(opcode, 7, 11), 
                       get_segment(opcode, 15, 19), 
                       get_segment(opcode, 20, 24), 
                       get_segment(opcode, 12, 14), 
                       get_segment(opcode, 25, 31));
      case 0b0010011: // I-type arithmetic
      return execute_I_arith(cpu.state, get_segment(opcode, 0, 6),
                            get_segment(opcode, 7, 11),
                            get_segment(opcode, 15, 19),
                            sign_extend(get_segment(opcode, 20, 31), 12),
                            get_segment(opcode, 12, 14));
      case 0b0000011: // I-type load
      return execute_I_load(cpu.state, get_segment(opcode, 0, 6),
                            get_segment(opcode, 7, 11),
                            get_segment(opcode, 15, 19),
                            sign_extend(get_segment(opcode, 20, 31), 12),
                            get_segment(opcode, 12, 14));
      case 0b0010111: // auipc
      case 0b0110111: // lui
      return execute_U(cpu.state, get_segment(opcode, 0, 6),
                       get_segment(opcode, 7, 11),
                       sign_extend(get_segment(opcode, 12, 31), 20));
      case 0b1100011: // B-type
      return execute_B(cpu.state, get_segment(opcode, 0, 6),
                       get_segment(opcode, 15, 19),
                       get_segment(opcode, 20, 24),
                       get_segment(opcode, 12, 14),
                       opcode);
      case 0b0100011: // S-type
      return execute_S(cpu.state, get_segment(opcode, 0, 6),
                       get_segment(opcode, 15, 19),
                       get_segment(opcode, 20, 24),
                       sign_extend(get_segment(opcode, 20, 31), 12),
                       get_segment(opcode, 12, 14));
      case 0b1101111: // J-type
      return execute_J(cpu.state, get_segment(opcode, 0, 6),
                       get_segment(opcode, 7, 11),
                       opcode);
      default:
      return false;
    }
  }
};
}
