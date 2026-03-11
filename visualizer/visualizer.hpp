#pragma once

#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <bits/stdc++.h>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>
#include "../cpu/cpu.hpp"
#include "int_reg_visualizer.hpp"
#include "text_visualizer.hpp"
#include "data_visualizer.hpp"
#include "console_visualizer.hpp"

namespace visualizer {
using namespace ftxui;
static void Visualizer(cpu::CPU& cpu, uint64_t text_start, uint64_t text_len) {
    auto screen = ScreenInteractive::FullscreenAlternateScreen();
    auto Int_Reg = IntRegisterVisualizer(cpu);
    auto Text_Vis = TextVisualizer(cpu);
    auto Data_Vis = DataVisualizer(cpu, 0x10010000, 4);
    auto Console_Vis = ConsoleVisualizer(cpu);
    auto handler = CatchEvent([&](Event e) {
      if (e == Event::F2) {
        try {
          Int_Reg.pre_step();
          Text_Vis.pre_step();
          cpu.step();
        } catch (std::exception& in) {
          Console_Vis.push_str(std::string("\nerror: ") + in.what(), Color::RGB(255, 50, 50));
        }
        return true;
        }
      return false;
    });
    if (dynamic_cast<cpu::CPU_EnvCall*>(cpu.features["env"].get())) {
      auto env = static_cast<cpu::CPU_EnvCall*>(cpu.features["env"].get());
      env->handlers[10] = [&screen](cpu::CPU& cpu) {
        screen.Exit();
      };
    }
    auto layout = Container::Horizontal({
      Container::Vertical({
        Text_Vis(),
        Data_Vis(),
        Console_Vis(),
      }),
      Int_Reg()
    });
    screen.Loop(layout | handler);
    screen.Exit();
  }
}
