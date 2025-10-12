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
  uint32_t get_op_I_load(std::string op) {
    if (op == "lb") return 0b0000011 | (0b000 << 12);
    if (op == "lh") return 0b0000011 | (0b001 << 12);
    if (op == "lw") return 0b0000011 | (0b010 << 12);
    if (op == "lbu") return 0b0000011 | (0b100 << 12);
    if (op == "lhu") return 0b0000011 | (0b101 << 12);
    throw "Unknown operator";
  }

  uint32_t build_instruction_I_load(std::string op, uint8_t rd, uint8_t rs1, int64_t imm) {
    if (imm > 4095) throw "Immediate out of range";
    if (imm < -4096) throw "Immediate out of range";
    uint32_t result = 0;
    result |= (rd << 7);
    result |= (rs1 << 15);
    result |= (imm << 20);
    result |= get_op_I_load(op);
    return result;
  }
  uint32_t process_asm_I_load(std::string line, calculator& calc) {
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
    return build_instruction_I_load(op, get_reg(rd), get_reg(rs1), calc(imm));
  }
}


