#pragma once

#include "calculator.hpp"
#include "common.hpp"
#include <cstdint>
#include <string>

#pragma once

#include "calculator.hpp"
#include "common.hpp"
#include <cstdint>
#include <string>

namespace _assembler {
  uint32_t get_op_S(std::string op) {
    if (op == "sb") return 0b0100011 | (0b000 << 12);
    if (op == "sh") return 0b0100011 | (0b001 << 12);
    if (op == "sw") return 0b0100011 | (0b010 << 12);
    throw "Unknown operator";
  }

  uint32_t build_instruction_S(std::string op, uint8_t rs2, uint8_t rs1, int64_t imm) {
    if (imm > 4095) throw "Immediate out of range";
    if (imm < -4096) throw "Immediate out of range";
    uint32_t result = 0;
    result |= (rs1 << 15);
    result |= (rs2 << 20);
    result |= (imm & 0b11111) << 7;
    result |= (imm >> 5) << 25;
    result |= get_op_S(op);
    return result;
  }
  uint32_t process_asm_S(std::string line, calculator& calc) {
    std::string op; {
      int temp = line.find(' ');
      op = line.substr(0, temp);
      line = line.substr(temp+1);
      while (line[0] == ' ' || line[0] == '\t') line = line.substr(1);
    }
    std::string rs2; {
      int temp = line.find(',');
      rs2 = line.substr(0, temp);
      while (rs2[rs2.size()-1] == ' ' || rs2[rs2.size()-1] == '\t') rs2 = rs2.substr(0,rs2.size()-1);
      line = line.substr(temp+1);
      while (line[0] == ' ' || line[0] == '\t') line = line.substr(1);
    }
    std::string imm; {
      int temp = line.rfind("(");
      imm = line.substr(0, temp);
      line = line.substr(temp+1);
      while (line[0] == ' ' || line[0] == '\t') line = line.substr(1);
    }
    std::string rs1; {
      int temp = line.rfind(")");
      rs1 = line.substr(0, temp);
      line = line.substr(temp+1);
      while (line[0] == ' ' || line[0] == '\t') line = line.substr(1);
    }
    return build_instruction_S(op, get_reg(rs2), get_reg(rs1), calc(imm));
  }
}



