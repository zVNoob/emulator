#pragma once

#include <map>
#include <string>
#include <cstdint>
#include <stdexcept>
#include "asm_B.hpp"
#include "asm_I_load.hpp"
#include "asm_R.hpp"
#include "asm_I.hpp"
#include "asm_U.hpp"
#include "asm_S.hpp"
#include "calculator.hpp"

namespace _assembler {
  class InvalidInstruction : public std::runtime_error {
  public:
    InvalidInstruction(std::string line,std::string msg) : std::runtime_error(msg + ": " + line) {}
  };
  uint32_t build_instruction_J(std::string op, uint8_t rd, int64_t imm) {
    if (imm > 2097151) throw "Immediate out of range";
    if (imm < -2097152) throw "Immediate out of range";
    if (imm % 4) throw "Immediate unaligned";
    uint32_t result = 0;
    result |= (rd << 7);
    result |= (imm >> 20) << 31;
    result |= ((imm >> 1) & 0b1111111111) << 21;
    result |= ((imm >> 11) & 0b1) << 20;
    result |= ((imm >> 12) & 0b11111111) << 12;
    result |= 0b1101111;
    return result;
  }
  uint32_t process_asm_J(std::string line, calculator& calc) {
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
    return build_instruction_J(op, get_reg(rd), calc("("+imm+") - ."));
  }
  uint32_t process_asm(std::string line,const std::map<std::string, uint64_t>& labels, calculator& calc) {
    try {
      if (line.find("add ") != std::string::npos) return process_asm_R(line);
      if (line.find("sub ") != std::string::npos) return process_asm_R(line);
      if (line.find("sll ") != std::string::npos) return process_asm_R(line);
      if (line.find("slt ") != std::string::npos) return process_asm_R(line);
      if (line.find("sltu ") != std::string::npos) return process_asm_R(line);
      if (line.find("and ") != std::string::npos) return process_asm_R(line);
      if (line.find("or ") != std::string::npos) return process_asm_R(line);
      if (line.find("xor ") != std::string::npos) return process_asm_R(line);
      if (line.find("srl ") != std::string::npos) return process_asm_R(line);
      if (line.find("sra ") != std::string::npos) return process_asm_R(line);

      if (line.find("addi ") != std::string::npos) return process_asm_I(line, calc);
      if (line.find("slti ") != std::string::npos) return process_asm_I(line, calc);
      if (line.find("sltiu ") != std::string::npos) return process_asm_I(line, calc);
      if (line.find("xori ") != std::string::npos) return process_asm_I(line, calc);
      if (line.find("ori ") != std::string::npos) return process_asm_I(line, calc);
      if (line.find("andi ") != std::string::npos) return process_asm_I(line, calc);
      if (line.find("slli ") != std::string::npos) return process_asm_I(line, calc);
      if (line.find("srli ") != std::string::npos) return process_asm_I(line, calc);
      if (line.find("srai ") != std::string::npos) return process_asm_I(line, calc);
      if (line.find("jalr ") != std::string::npos) return process_asm_I(line, calc);

      if (line.find("lui ") != std::string::npos) return process_asm_U(line, calc);
      if (line.find("auipc ") != std::string::npos) return process_asm_U(line, calc);

      if (line.find("beq ") != std::string::npos) return process_asm_B(line, calc);
      if (line.find("bne ") != std::string::npos) return process_asm_B(line, calc);
      if (line.find("blt ") != std::string::npos) return process_asm_B(line, calc);
      if (line.find("bge ") != std::string::npos) return process_asm_B(line, calc);
      if (line.find("bltu ") != std::string::npos) return process_asm_B(line, calc);
      if (line.find("bgeu ") != std::string::npos) return process_asm_B(line, calc);

      if (line.find("lb ") != std::string::npos) return process_asm_I_load(line, calc);
      if (line.find("lh ") != std::string::npos) return process_asm_I_load(line, calc);
      if (line.find("lw ") != std::string::npos) return process_asm_I_load(line, calc);
      if (line.find("lbu ") != std::string::npos) return process_asm_I_load(line, calc);
      if (line.find("lhu ") != std::string::npos) return process_asm_I_load(line, calc);

      if (line.find("sb ") != std::string::npos) return process_asm_S(line, calc);
      if (line.find("sh ") != std::string::npos) return process_asm_S(line, calc);
      if (line.find("sw ") != std::string::npos) return process_asm_S(line, calc);
      
      if (line.find("jal ")!= std::string::npos) return process_asm_J(line, calc);
    }
    catch (const char* s) {
      throw InvalidInstruction(line, s);
    }
    throw InvalidInstruction(line, "Invalid instruction");
  }

}
