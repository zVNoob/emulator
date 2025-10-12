#pragma once

#include "common.hpp"

namespace _assembler {
  uint32_t get_op_R(std::string op) {
    if (op == "add") return 0b0110011 | (0b000 << 12) | (0b0000000 << 25);
    if (op == "sub") return 0b0110011 | (0b000 << 12) | (0b0100000 << 25);
    if (op == "sll") return 0b0110011 | (0b001 << 12) | (0b0000000 << 25);
    if (op == "slt") return 0b0110011 | (0b010 << 12) | (0b0000000 << 25);
    if (op == "sltu") return 0b0110011 | (0b011 << 12) | (0b0000000 << 25);
    if (op == "xor") return 0b0110011 | (0b100 << 12) | (0b0000000 << 25);
    if (op == "srl") return 0b0110011 | (0b101 << 12) | (0b0000000 << 25);
    if (op == "sra") return 0b0110011 | (0b101 << 12) | (0b0100000 << 25);
    if (op == "or") return 0b0110011 | (0b110 << 12) | (0b0000000 << 25);
    if (op == "and") return 0b0110011 | (0b111 << 12) | (0b0000000 << 25);
    throw "Unknown operator";
  }
  uint32_t build_instruction_R(std::string op, uint8_t rd, uint8_t rs1, uint8_t rs2) {
    uint32_t result = 0;
    result |= (rd << 7);
    result |= (rs1 << 15);
    result |= (rs2 << 20);
    result |= get_op_R(op);
    return result;
  }
  uint32_t process_asm_R(std::string line) {
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
    std::string rs2; {
      rs2 = line;
      while (rs2[rs2.size()-1] == ' ' || rs2[rs2.size()-1] == '\t') rs2 = rs2.substr(0,rs2.size()-1);
    }
    return build_instruction_R(op, get_reg(rd), get_reg(rs1), get_reg(rs2));
  }
}
