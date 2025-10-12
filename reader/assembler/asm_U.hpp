#pragma once

#include "calculator.hpp"
#include "common.hpp"
#include <cstdint>
#include <string>

namespace _assembler {
  uint32_t get_op_U(std::string op) {
    if (op == "lui") return 0b0110111;
    if (op == "auipc") return 0b0010111;
    throw "Unknown operator";
  }
  uint32_t build_instruction_U(std::string op, uint8_t rd, int64_t imm) {
    if (imm > 1048575) throw "Immediate out of range";
    if (imm < -1048576) throw "Immediate out of range";
    uint32_t result = 0;
    result |= (rd << 7);
    result |= (imm << 12);
    result |= get_op_U(op);
    return result;
  }
  uint32_t process_asm_U(std::string line, calculator& calc) {
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
      imm = line;
      while (imm[imm.size()-1] == ' ' || imm[imm.size()-1] == '\t') imm = imm.substr(0,imm.size()-1);
    }
    return build_instruction_U(op, get_reg(rd), calc(imm));
  }
}


