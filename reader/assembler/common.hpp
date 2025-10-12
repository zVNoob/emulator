#pragma once

#include <string>
#include <cstdint>

namespace _assembler {
  uint8_t get_reg(std::string reg_name) {
    if (reg_name[0] == 'x') return std::stoi(reg_name.substr(1));
    if (reg_name == "zero") return 0;
    if (reg_name == "ra") return 1;
    if (reg_name == "sp") return 2;
    if (reg_name == "gp") return 3;
    if (reg_name == "tp") return 4;
    if (reg_name == "t0") return 5;
    if (reg_name == "t1") return 6;
    if (reg_name == "t2") return 7;
    if (reg_name == "s0") return 8;
    if (reg_name == "s1") return 9;
    if (reg_name == "a0") return 10;
    if (reg_name == "a1") return 11;
    if (reg_name == "a2") return 12;
    if (reg_name == "a3") return 13;
    if (reg_name == "a4") return 14;
    if (reg_name == "a5") return 15;
    if (reg_name == "a6") return 16;
    if (reg_name == "a7") return 17;
    if (reg_name == "s2") return 18;
    if (reg_name == "s3") return 19;
    if (reg_name == "s4") return 20;
    if (reg_name == "s5") return 21;
    if (reg_name == "s6") return 22;
    if (reg_name == "s7") return 23;
    if (reg_name == "s8") return 24;
    if (reg_name == "s9") return 25;
    if (reg_name == "s10") return 26;
    if (reg_name == "s11") return 27;
    if (reg_name == "t3") return 28;
    if (reg_name == "t4") return 29;
    if (reg_name == "t5") return 30;
    if (reg_name == "t6") return 31;
   throw 0;
  }
  uint64_t to_int_with_base(const std::string& s, int base) {
    uint64_t ret = 0;
   for (char c : s) {
      if (c >= '0' && c <= '9') ret = ret * base + (c - '0');
      if (c >= 'a' && c <= 'f') ret = ret * base + (c - 'a' + 10);
      if (c >= 'A' && c <= 'F') ret = ret * base + (c - 'A' + 10);
    }
    return ret;
  }
  uint64_t to_int(const std::string& s) {
    if (s[0] == '0' && s[1] == 'x') return to_int_with_base(s.substr(2), 16);
    if (s[0] == '0' && s[1] == 'b') return to_int_with_base(s.substr(2), 2);
    if (s[0] == '0' && s[1] == 'o') return to_int_with_base(s.substr(2), 8);
    return to_int_with_base(s, 10);
  }
}
