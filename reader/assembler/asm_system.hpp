#pragma once

#include <string>
#include <cstdint>

namespace _assembler {

  uint32_t process_asm_system(std::string& line) {
    if (line == "ecall ") return 0x00000073;
    if (line == "ebreak ") return 0x00100073;
    throw "Unknown assembly";
  }
}
