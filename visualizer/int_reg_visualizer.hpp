#pragma once

#include <algorithm>
#include <cstdint>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <bits/stdc++.h>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>
#include <sstream>
#include <sys/types.h>
#include "../cpu/cpu.hpp"
#include <memory>

namespace visualizer {
using namespace ftxui;

static ButtonOption RegStyle() {
  ButtonOption option;
  option.transform = [](const EntryState& s) {
    std::string name = s.label;
    return text(name);
  };
  return option;
}

class SingleIntRegisterVisualizer {
  uint64_t* value;
  uint64_t* old_value;
  std::string name;
  std::shared_ptr<bool> is_hex;
public:
  SingleIntRegisterVisualizer(std::string name, uint64_t* value,uint64_t* old_value) {
    this->name = name;
    this->value = value;
    this->old_value = old_value;
    is_hex = std::make_shared<bool>(true);
  }
  auto operator()() {
      std::string n = name;
      uint64_t* v = value;
      uint64_t* ov = old_value;
      auto h = is_hex;
      auto b = Button(n, [h]() { *h = !*h;}, RegStyle());
      return Renderer(b,[b,n,v,ov,h]() {
        std::stringstream ss;
        if (*h) ss << std::hex << std::setfill('0');
        ss << std::setw(16) << *v;
        auto res = hbox({
          b->Render() | color(Color::RGB(255, 255, 255)),
          text(": "),
          text(ss.str()) | ((*v == *ov)? color(Color::RGB(100, 100, 255)): color(Color::RGB(255, 100, 100))),
        }) | align_right;
        return res;
      });
  }
};

// static Component SingleRegisterVisualizer(std::string name, uint64_t* value) {
//
//   return Button(name + ": " + std::to_string(*value), [&]() { }, RegStyle());
// }

class IntRegisterVisualizer {
  std::vector<SingleIntRegisterVisualizer> arr;
  std::vector<Component> res_arr1;
  Component temp;
  std::vector<uint64_t> old_val;
  cpu::CPU* cpu;
  public:
  IntRegisterVisualizer(cpu::CPU& cpu) {
    old_val.resize(33);
    this->cpu = &cpu;
    arr = {
      SingleIntRegisterVisualizer("zero", &cpu.state.regs[0], &old_val[0]),
      SingleIntRegisterVisualizer("ra", &cpu.state.regs[1], &old_val[1]),
      SingleIntRegisterVisualizer("sp", &cpu.state.regs[2], &old_val[2]),
      SingleIntRegisterVisualizer("gp", &cpu.state.regs[3], &old_val[3]),
      SingleIntRegisterVisualizer("tp", &cpu.state.regs[4], &old_val[4]),
      SingleIntRegisterVisualizer("t0", &cpu.state.regs[5], &old_val[5]),
      SingleIntRegisterVisualizer("t1", &cpu.state.regs[6], &old_val[6]),
      SingleIntRegisterVisualizer("t2", &cpu.state.regs[7], &old_val[7]),
      SingleIntRegisterVisualizer("s0", &cpu.state.regs[8], &old_val[8]),
      SingleIntRegisterVisualizer("s1", &cpu.state.regs[9], &old_val[9]),
      SingleIntRegisterVisualizer("a0", &cpu.state.regs[10], &old_val[10]),
      SingleIntRegisterVisualizer("a1", &cpu.state.regs[11], &old_val[11]),
      SingleIntRegisterVisualizer("a2", &cpu.state.regs[12], &old_val[12]),
      SingleIntRegisterVisualizer("a3", &cpu.state.regs[13], &old_val[13]),
      SingleIntRegisterVisualizer("a4", &cpu.state.regs[14], &old_val[14]),
      SingleIntRegisterVisualizer("a5", &cpu.state.regs[15], &old_val[15]),
      SingleIntRegisterVisualizer("a6", &cpu.state.regs[16], &old_val[16]),
      SingleIntRegisterVisualizer("a7", &cpu.state.regs[17], &old_val[17]),
      SingleIntRegisterVisualizer("s2", &cpu.state.regs[18], &old_val[18]),
      SingleIntRegisterVisualizer("s3", &cpu.state.regs[19], &old_val[19]),
      SingleIntRegisterVisualizer("s4", &cpu.state.regs[20], &old_val[20]),
      SingleIntRegisterVisualizer("s5", &cpu.state.regs[21], &old_val[21]),
      SingleIntRegisterVisualizer("s6", &cpu.state.regs[22], &old_val[22]),
      SingleIntRegisterVisualizer("s7", &cpu.state.regs[23], &old_val[23]),
      SingleIntRegisterVisualizer("s8", &cpu.state.regs[24], &old_val[24]),
      SingleIntRegisterVisualizer("s9", &cpu.state.regs[25], &old_val[25]),
      SingleIntRegisterVisualizer("s10", &cpu.state.regs[26], &old_val[26]),
      SingleIntRegisterVisualizer("s11", &cpu.state.regs[27], &old_val[27]),
      SingleIntRegisterVisualizer("t3", &cpu.state.regs[28], &old_val[28]),
      SingleIntRegisterVisualizer("t4", &cpu.state.regs[29], &old_val[29]),
      SingleIntRegisterVisualizer("t5", &cpu.state.regs[30], &old_val[30]),
      SingleIntRegisterVisualizer("t6", &cpu.state.regs[31], &old_val[31]),
      SingleIntRegisterVisualizer("pc", &cpu.state.pc, &old_val[32]),
    };    
    std::transform(arr.begin(), arr.end(), std::back_inserter(res_arr1), [](SingleIntRegisterVisualizer& x) { return x(); });
    temp = Container::Vertical(res_arr1);
  }
  void update_old_val() {
    for (int i = 0; i < 33; i++) {
      old_val[i] = cpu->state.regs[i];
    }
  }
  auto operator()() {
    auto t = temp;
    auto* ov = &old_val;
    auto* c = cpu;
    return Renderer(temp, [ov,c,t]() { 
      auto res = window(text("Registers") | color(Color::RGB(200, 255, 100))
                        ,t->Render() | vscroll_indicator | frame) | size(WIDTH, LESS_THAN, 24) | size(HEIGHT, LESS_THAN, 33);
      return res;
    });
  }
};

}

