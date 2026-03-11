#pragma once

#include "calculator.hpp"
#include "common.hpp"
#include <list>
#include <stdexcept>

namespace _assembler {
void replace_single(std::list<std::string>& text_asm, 
                    std::list<std::string>::iterator current_line, 
                    std::string new_str) {
  *current_line = new_str;
}
void replace_double(std::list<std::string>& text_asm,
                    std::list<std::string>::iterator current_line,
                    std::string new_str1, 
                    std::string new_str2) {
  *current_line = new_str1;
  text_asm.insert(++current_line, new_str2);
}
void decode_pesudo_asm(std::list<std::string>& text_asm, std::list<std::string>::iterator current_line, calculator& calc) {
  if (current_line->find("nop ") == 0) 
    replace_single(text_asm, current_line, "addi x0, x0, 0");
  else if (current_line->find("mv ") == 0) {
    std::string rd = current_line->substr(3, current_line->find(",") - 3);
    std::string rs1 = current_line->substr(current_line->find(",") + 1);
    replace_single(text_asm, current_line, "add " + rd + ", " + rs1 + ", x0");
  } else if (current_line->find("ble ") == 0) {
    std::string rs1 = current_line->substr(4, current_line->find(",") - 4);
    std::string rs2 = current_line->substr(current_line->find(",") + 1);
    std::string label = rs2.substr(current_line->find(",") + 2);
  }
  else if (current_line->find("j ") == 0) 
    replace_single(text_asm, current_line, "jal x0," + current_line->substr(2));
  else if (current_line->find("jr ") == 0) 
    replace_single(text_asm, current_line, "jalr x0," + current_line->substr(3));
  else if (current_line->find("ret ") == 0) 
    replace_single(text_asm, current_line, "jalr x0,x1,0");
  else if (current_line->find("call ") == 0) {
    std::string temp = current_line->substr(5);
    while (temp[0] == ' ' || temp[0] == '\t') temp = temp.substr(1);
    try {
      int64_t val = calc(temp);
      if (val > 1048575) throw std::invalid_argument("out of range");
      if (val < -1048576) throw std::invalid_argument("out of range");
      replace_single(text_asm, current_line, 
                     "jal x1, " + std::to_string(val));
    } catch (std::invalid_argument e) {
      replace_double(text_asm, current_line, 
                   "auipc ra, (((" + temp + ") - .) >> 12) + (((" + temp + ") - .) >> 11 & 1)",
                   "jalr ra, ra , ((" + temp + ") - . + 4) & 4095"
                     );
    }
  }
  else if (current_line->find("li ") == 0) {
    std::string s = current_line->substr(3);
    while (s[0] == ' ' || s[0] == '\t') s = s.substr(1);
    std::string reg = s.substr(0, s.find(","));
    std::string temp = s.substr(s.find(",") + 1);
    while (temp[0] == ' ' || temp[0] == '\t') temp = temp.substr(1);
    try {
      int64_t val = calc(temp);
      if (val > 4095) throw std::invalid_argument("out of range");
      if (val < -4096) throw std::invalid_argument("out of range");
      replace_single(text_asm, current_line, 
                     "addi " + reg + ", x0, " + std::to_string(val));
    } catch (std::invalid_argument e) {
      replace_double(text_asm, current_line, 
                     "lui " + reg + ", ((" +temp +") >> 12) + (("+temp +")>> 11 & 1)",
                     "addi " + reg + ", " + reg + " , (" + temp + ") & 4095");
    }
  } else if (current_line->find("la ") == 0) {
    std::string s = current_line->substr(3);
    while (s[0] == ' ' || s[0] == '\t') s = s.substr(1);
    std::string reg = s.substr(0, s.find(","));
    std::string temp = s.substr(s.find(",") + 1);
    while (temp[0] == ' ' || temp[0] == '\t') temp = temp.substr(1);
    replace_double(text_asm, current_line,
                   "auipc " + reg + ", (((" + temp + ") - .) >> 12) + (((" + temp + ") - .) >> 11 & 1)",
                   "addi " + reg + ", " + reg + " , ((" + temp + ") - . + 4) & 4095"
                   );
  }
}
}
