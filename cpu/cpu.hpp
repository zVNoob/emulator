#pragma once

#include "../memory/memory.hpp"
#include <cstdint>
#include <memory>

namespace cpu {

struct CPU_State {
  uint64_t pc = 0;
  uint64_t regs[32] = {0};
  std::shared_ptr<memory::Memory> mem;
};

class CPU;

class CPU_Interrupt : public std::exception {};

class UnknownInstruction : public CPU_Interrupt {
public:
  std::string msg;
  UnknownInstruction(uint32_t opcode) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(8) << opcode;
    msg = "Unknown instruction: 0x";
    msg += ss.str();
  }
  const char* what() const throw() {
    return msg.c_str();
  }
};

class CPU_Feature {
protected:
  uint32_t get_segment(uint32_t reg_value, uint8_t lower, uint8_t upper) {
    return (reg_value >> lower) & ((1 << (upper - lower + 1)) - 1);
  }
public:
  bool enabled = true;
  virtual bool execute(uint32_t opcode,CPU& state) = 0;
  virtual std::string debug(std::string request) {return "";}
  virtual bool handle_interrupt(CPU& state, std::shared_ptr<CPU_Interrupt> interrupt) {return false;}
};

class CPU {
public:
  std::map<std::string,std::unique_ptr<CPU_Feature>> features;
  CPU_State state;
  bool running = true;
  void step() {
    state.regs[0] = 0;
    if (!running) return;
    uint32_t opcode = state.mem->read(state.pc, 4);
    try {
      for (auto& feature : features) {
        if (feature.second->execute(opcode, *this)) {
          state.pc += 4;
          return;
        }
      }
      throw std::make_shared<UnknownInstruction>(opcode);
    } catch (std::shared_ptr<CPU_Interrupt> interrupt) {
      for (auto& feature : features) {
        if (feature.second->handle_interrupt(*this, interrupt)) {
          return;
        }
      }
      throw interrupt;
    }
  }
  void run() {
    while (true) {
      step();
    }
  }
};

}
