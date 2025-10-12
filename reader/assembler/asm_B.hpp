#pragma once

#include "calculator.hpp"
#include "common.hpp"
#include <string>
#include <map>

namespace _assembler {
  uint32_t get_op_B(std::string op) {
    if (op == "beq") return 0b1100011 | (0b000 << 12);
    if (op == "bne") return 0b1100011 | (0b001 << 12);
    if (op == "blt") return 0b1100011 | (0b100 << 12);
    if (op == "bge") return 0b1100011 | (0b101 << 12);
    if (op == "bltu") return 0b1100011 | (0b110 << 12);
    if (op == "bgeu") return 0b1100011 | (0b111 << 12);
    throw "Unknown operator";
  }

  uint32_t get_imm_B(std::string imm) {
    auto temp = std::stoi(imm);
    if (temp < 4095) throw 0;
    if (temp > -4096) throw 0;
    return temp;
  }
  uint32_t build_imm_B(int64_t imm) {
    if (imm > 8191) throw "Branch target out of range";
    if (imm < -8192) throw "Branch target out of range";
    uint32_t result = 0;
    result |= ((imm >> 11) & 0b1) << 7;
    result |= ((imm >> 1) & 0b1111) << 8;
    result |= ((imm >> 12) & 0b1) << 31;
    result |= ((imm >> 5) & 0b111111) << 25;
    return result;
  }
  uint32_t build_instruction_B(std::string op, uint8_t rs1, uint8_t rs2, int64_t imm) {
    uint32_t result = 0;
    result |= (rs1 << 15);
    result |= (rs2 << 20);
    result |= build_imm_B(imm);
    result |= get_op_B(op);
    return result;
  }
  uint32_t process_asm_B(std::string line,calculator& calc) {
    std::string op; {
      int temp = line.find(' ');
      op = line.substr(0, temp);
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
      int temp = line.find(',');
      rs2 = line.substr(0, temp);
      while (rs2[rs2.size()-1] == ' ' || rs2[rs2.size()-1] == '\t') rs2 = rs2.substr(0,rs2.size()-1);
      line = line.substr(temp+1);
      while (line[0] == ' ' || line[0] == '\t') line = line.substr(1);
    }
    std::string label; {
      label = line;
      while (label[label.size()-1] == ' ' || label[label.size()-1] == '\t') label = label.substr(0,label.size()-1);
    }    
    return build_instruction_B(op, get_reg(rs1), get_reg(rs2), calc("("+label+") - ." ));
    
  }
}
