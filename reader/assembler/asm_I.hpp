#pragma once

#include "calculator.hpp"
#include "common.hpp"
#include <cstdint>
#include <string>

namespace _assembler {
  uint32_t get_op_I(std::string op) {
    if (op == "addi") return 0b0010011 | (0b000 << 12);
    if (op == "slti") return 0b0010011 | (0b010 << 12);
    if (op == "sltiu") return 0b0010011 | (0b011 << 12);
    if (op == "xori") return 0b0010011 | (0b100 << 12);
    if (op == "ori") return 0b0010011 | (0b110 << 12);
    if (op == "andi") return 0b0010011 | (0b111 << 12);
    if (op == "slli") return 0b0010011 | (0b001 << 12) | (0b0000000 << 25);
    if (op == "srli") return 0b0010011 | (0b101 << 12) | (0b0000000 << 25);
    if (op == "srai") return 0b0010011 | (0b101 << 12) | (0b0100000 << 25);
    if (op == "jalr") return 0b1100111 | (0b000 << 12);
    throw "Unknown operator";
  }

  uint32_t build_instruction_I(std::string op, uint8_t rd, uint8_t rs1, int64_t imm) {
    if (imm > 4095) throw "Immediate out of range";
    if (imm < -4096) throw "Immediate out of range";
    uint32_t result = 0;
    result |= (rd << 7);
    result |= (rs1 << 15);
    result |= (imm << 20);
    result |= get_op_I(op);
    return result;
  }
  uint32_t process_asm_I(std::string line, calculator& calc) {
    std::string op; {
      int temp = line.find(' ');
      op = line.substr(0, temp);
      line = line.substr(temp+1);
      while (line[0] == ' ' || line[0] == '\t') line = line.substr(1);
    }
    std::string rd; {
      int temp = line.find(',');
      rd = line.substr(0, temp);
      while (rd[rd.size()-1] == ' ' || rd[rd.size()-1] == '\t') rd = rd.substr(0,rd.size()-1);
      line = line.substr(temp+1);
      while (line[0] == ' ' || line[0] == '\t') line = line.substr(1);
    }
    std::string rs1; {
      int temp = line.find(',');
      rs1 = line.substr(0, temp);
      while (rs1[rs1.size()-1] == ' ' || rs1[rs1.size()-1] == '\t') rs1 = rs1.substr(0,rs1.size()-1);
      line = line.substr(temp+1);
      while (line[0] == ' ' || line[0] == '\t') line = line.substr(1);
    }
    std::string imm; {
      imm = line;
      while (imm[imm.size()-1] == ' ' || imm[imm.size()-1] == '\t') imm = imm.substr(0,imm.size()-1);
    }
    return build_instruction_I(op, get_reg(rd), get_reg(rs1), calc(imm));
  }
}

