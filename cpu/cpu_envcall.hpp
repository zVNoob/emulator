#pragma once

#include "cpu.hpp"
#include <functional>
#include <map>

namespace cpu {
  class CPU_EnvCall : public cpu::CPU_Feature {
  public:
    std::map<int, std::function<void(CPU&)>> handlers;
    bool execute(uint32_t opcode,CPU& state) override {
      if (get_segment(opcode, 0, 6) != 0b1110011) return false;
      if (get_segment(opcode,12,14) != 0b000) return false;
      if (get_segment(opcode,15,24) != 0) return false;
      int imm = get_segment(opcode, 25, 31);
      if (imm == 0) {
        if (handlers[state.state.regs[17]]) // environment call
          handlers[state.state.regs[17]](state);
        return true;
      } else if (imm == 1) {
        if (handlers[-1]) // debugger call
          handlers[-1](state);
        return true;
      }
      return false;
    }
  };
}
