#pragma once

#include "../reader.hpp"
#include <cstdint>
#include <fstream>
#include <istream>
#include <list>
#include <stdexcept>
#include <string>
#include "asm.hpp"
#include "calculator.hpp"
#include "common.hpp"
#include "pesudo_asm.hpp"
#include <map>
#include <vector>

namespace reader {
  class assembler : public Reader {
  // Asm source
  uint64_t data_start;
  std::list<std::string> data_asm;
  uint64_t text_start;
  std::list<std::string> text_asm;
  // Translated memory
  std::vector<uint8_t> data_memory;
  std::vector<uint8_t> text_memory;
  
  int region = 0;
  // Calculator
  _assembler::calculator calc;
  // Include stack
  std::istream& input;
  std::list<std::ifstream> include_stack;
  bool get_line(std::string& s) {
    while (include_stack.size() > 0) {
      if (!std::getline(include_stack.front(), s)) {
        include_stack.pop_front();
        continue;
      }
      return true;
    }
    return !(!(std::getline(input, s)));
    
  }
  // First pass (preprocess marking and include directives)
  void push_asm_line(std::string&& line) {
    if (region == 1) data_asm.push_back(std::move(line));
    else if (region == 2) text_asm.push_back(std::move(line));
    else throw std::runtime_error("Invalid region");
  }
  bool preprocess_directives(std::string& line) {
    if (line.find(".data ") != std::string::npos) {
      region = 1;
      std::string s = line.substr(6);
      while (s[0] == ' ' || s[0] == '\t') s = s.substr(1);
      if (s.size() > 0) data_start = _assembler::to_int(s);
      return true;
    }
    if (line.find(".text ") != std::string::npos) {
      region = 2;
      std::string s = line.substr(6);
      while (s[0] == ' ' || s[0] == '\t') s = s.substr(1);
      if (s.size() > 0) text_start = _assembler::to_int(s);
      return true;
    }
    if (line.find(".include ") != std::string::npos) {
      std::string s = line.substr(9);
      while (s[0] == ' ' || s[0] == '\t') s = s.substr(1);
      s = s.substr(1, s.size()-2);
      std::ifstream file(s);
      include_stack.push_front(std::move(file));
      return true;
    }
    return false;
  }
  // Second pass (process data directives and get labels positions)
  std::map<std::string, uint64_t> labels;
  uint64_t process_directives(const std::string& line) {
    if (line.find(".ascii ") != std::string::npos) {
      std::string s = line.substr(7);
      while (s[0] == ' ' || s[0] == '\t') s = s.substr(1);
      s = s.substr(1, s.size()-2);
      for (char c : s) data_memory.push_back(c);
      return s.size();
    }
    if (line.find(".asciz ") != std::string::npos || line.find(".string ") != std::string::npos) {
      std::string s = line.substr(7);
      while (s[0] == ' ' || s[0] == '\t') s = s.substr(1);
      s = s.substr(1, s.size()-2);
      for (char c : s) data_memory.push_back(c);
      data_memory.push_back(0);
      return s.size() + 1;
    }
    if (line.find(".byte ") != std::string::npos) {
      std::string s = line.substr(6);
      while (s[0] == ' ' || s[0] == '\t') s = s.substr(1);
      int64_t val = _assembler::to_int(s);
      data_memory.push_back(val);
      return 1;
    }
    if (line.find(".word ") != std::string::npos) {
      std::string s = line.substr(6);
      while (s[0] == ' ' || s[0] == '\t') s = s.substr(1);
      uint64_t val = _assembler::to_int(s);
      data_memory.push_back(val & 0xff);
      data_memory.push_back((val >> 8) & 0xff);
      data_memory.push_back((val >> 16) & 0xff);
      data_memory.push_back((val >> 24) & 0xff);
      return 4;
    }
    if (line.find(".space ") != std::string::npos) {
      std::string s = line.substr(7);
      while (s[0] == ' ' || s[0] == '\t') s = s.substr(1);
      uint64_t val = _assembler::to_int(s);
      for (uint64_t i = 0; i < val; i++) data_memory.push_back(0);
      return val;
    }
    return 0;
  }
  void process_labels(std::string& line) {
    if (line.find(":") != std::string::npos) {
      std::string s = line.substr(0, line.find(":"));
      while (s[0] == ' ' || s[0] == '\t') s = s.substr(1);
      if (s.size() == 0) return;
      if (labels["."] % 4) throw std::runtime_error("Label not aligned");
      labels[s] = labels["."];
      line = line.substr(line.find(":") + 1);
      while (line[0] == ' ' || line[0] == '\t') if(line.size()) line = line.substr(1); else break;
    }
  }
  void process_data() {
    region = 1;
    labels["."] = data_start;
    for (auto& s : data_asm) {
      process_labels(s);
      if (s[0] == '.') labels["."] += process_directives(s);
    }
  }
  void process_text() {
    region = 2;
    labels["."] = text_start;
    for (auto it = text_asm.begin(); it != text_asm.end(); it++) {
      process_labels(*it);
      if (*it == "") continue;
      _assembler::decode_pesudo_asm(text_asm, it, calc);
      labels["."] += 4;
    }
  }
  void write32(uint32_t val) {
    text_memory.push_back((val >> 24) & 0xff);
    text_memory.push_back((val >> 16) & 0xff);
    text_memory.push_back((val >> 8) & 0xff);
    text_memory.push_back(val & 0xff);
  }
  void process_asm() {
    region = 2;
    labels["."] = text_start;
    for (auto it = text_asm.begin(); it != text_asm.end(); it++) {
      if (*it == "") continue;
      write32(_assembler::process_asm(*it, labels, calc));
      labels["."] += 4;
    }
  }
public:
  assembler(std::istream& input, uint64_t data_start = 0x10010000, uint64_t text_start = 0x00400000) : 
    input(input), calc(labels), text_start(text_start), data_start(data_start) {
    std::string line;
    while (get_line(line)) {
      auto cmt_start = line.find('#');
      if (cmt_start != std::string::npos) line = line.substr(0, cmt_start);
      while (line[0] == ' ' || line[0] == '\t') line = line.substr(1);
      if (line.size() == 0) continue;
      line += " ";
      if (preprocess_directives(line)) continue;
      push_asm_line(std::move(line));
      line = "";
    }
  }
  Region_Map operator()() override {
    process_data();
    process_text();
    process_asm();
    Read_Data text_seg = {
      .start = text_start,
      .size = text_memory.size(),
      .data = text_memory,
    };
    return {
      text_seg,
    };
  }
  };
}
