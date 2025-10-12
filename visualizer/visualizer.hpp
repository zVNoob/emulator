#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <bits/stdc++.h>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>
#include "../cpu/cpu.hpp"
#include "int_reg_visualizer.hpp"

namespace visualizer {
using namespace ftxui;
class Visualizer {
public:
  Visualizer(cpu::CPU& cpu) {
    auto screen = ScreenInteractive::FullscreenAlternateScreen();
    auto temp = IntRegisterVisualizer(cpu);
    auto handler = CatchEvent([&](Event e) {
      if (e == Event::Return) {
        try {
          temp.update_old_val();
          cpu.step();
        } catch (cpu::CPU_Interrupt& in) {

        }
          catch (...) {
          screen.Exit();
        }
        return true;
        }
      return false;
    });
    screen.Loop(temp() | handler);
  }
};
}
