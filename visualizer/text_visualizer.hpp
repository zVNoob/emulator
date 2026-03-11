#pragma once

#include "../cpu/cpu.hpp"
#include "disassembler.hpp"
#include <bits/stdc++.h>
#include <cstdint>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>
#include <memory>
#include <sstream>

namespace visualizer {
using namespace ftxui;

static ButtonOption TextRegStyle() {
  ButtonOption option;
  option.transform = [](const EntryState &s) { 
    auto res = text(s.label);
    if (s.focused)
      res |= bgcolor(Color::RGB(50, 50, 50));
    return res; 
  };
  return option;
}

static auto SingleTextVisualizer(cpu::CPU &cpu, uint64_t current_line) {
  std::string addr;
  {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(8) << current_line;
    addr = ss.str();
  }
  std::string opcode;
  {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(8)
       << cpu.state.mem->read(current_line, 4);
    opcode = ss.str();
  }
  auto b = Button(addr, []() {}, TextRegStyle());
  return Renderer(b, [b, opcode, &cpu, current_line]() {
    Color col = Color::RGB(100, 100, 100);
    if (cpu.state.pc == current_line)
      col = Color::RGB(255, 255, 255);
    auto temp = b->Render();
    auto res = hbox({b->Render() | color(col), text("| "),
                     text(opcode) | color(Color::RGB(255, 100, 255)),
                     text(" | "), text(disassembler::disassembler(opcode))});
    return res;
  });
}

class TextVisualizer {
  Component data;
  std::shared_ptr<uint64_t> current_max;
  cpu::CPU &cpu;
  bool changed = false;
  uint32_t range = 50;

public:
  uint64_t text_start = 0;
  uint64_t text_len = 0;
  TextVisualizer(cpu::CPU &cpu) : cpu(cpu) {
    data = Container::Vertical({});
    get_data();
  }
  void get_data() {
    if (cpu.state.pc <= text_len + text_start && cpu.state.pc >= text_start)
      return;
    data->DetachAllChildren();
    bool found = false;
    for (auto i = cpu.state.pc - range * 4; i < cpu.state.pc + range * 4; i += 4) {
      try {
        data->Add(SingleTextVisualizer(cpu, i));
        if (found == false)
          text_start = i;
        found = true;
        text_len = i - text_start;
      } catch (...) {
        continue;
      }
    }
  }
  void pre_step() {
    changed = true;
    get_data();
  }
  auto operator()() {
    auto c = data;
    auto &cp = cpu;
    auto &tx = text_start;
    auto &tl = text_len;
    auto &ch = changed;
    return Renderer(c, [c, &cp, &tx, &tl, &ch]() {
      std::stringstream ss;
      ss << std::hex << std::setfill('0') << std::setw(8) << tx;
      if (ch) {
        if ((cp.state.pc - tx)/4 < c->ChildCount())
          c->SetActiveChild(c->ChildAt((cp.state.pc - tx)/4));
        ch = false;
      }
      auto temp = c->Render();
      auto res = window(text("Code - 0x" + ss.str()) |
                            color(Color::RGB(200, 255, 100)),
                        temp | vscroll_indicator | frame);
      return res;
    });
  }
};
} // namespace visualizer
