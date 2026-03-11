#pragma once

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
#include <functional>
#include <string>
#include "../cpu/cpu.hpp"
#include "../cpu/cpu_envcall.hpp"

namespace visualizer {
using namespace ftxui;

enum InputMode {
  NONE,
  INTEGER,
  CHAR,
  STRING
};

auto ConsoleInput(InputMode& mode, cpu::CPU& cpu) {
  auto str = std::make_shared<std::string>();
  auto option = InputOption::Default();
  option.on_enter = [str, &mode, &cpu]() {
      if (mode == INTEGER) {
        cpu.state.regs[10] = std::stoll(*str.get());
      }
      if (mode == CHAR) {
        cpu.state.regs[10] = str->at(0);
      }
      if (mode == STRING) {
        uint64_t i = cpu.state.regs[10];
        try {
          for (auto& c : *str) {
            cpu.state.mem->write(i, c, 1);
            i++;
          }
          cpu.state.mem->write(i, 0, 1);
        } catch (std::exception& e) {
        }
      }
      mode = NONE;
      cpu.running = true;
      *str = "";
    };
  auto com = Input(str.get(),option);
  com |= CatchEvent([str, &mode, &cpu](Event event) {
    if (mode == INTEGER)
      return event.is_character() && !std::isdigit(event.character()[0]);
    if (mode == CHAR)
      return event.is_character() && str->size() == 1;
    if (mode == STRING) {
      return event.is_character() && str->size() >= cpu.state.regs[11];
    }
    return false;
  });
  auto enable = std::make_shared<bool>(false);
  auto res = Renderer(com, [com, &mode, &cpu]() {
    std::string input_mode;
    if (mode == INTEGER) {
      input_mode = "int";
    }
    if (mode == CHAR) {
      input_mode = "char";
    }
    if (mode == STRING) {
      input_mode = "str," + std::to_string(cpu.state.regs[11]);
    }
    return window(text("Input") | color(Color::RGB(200, 255, 100)),hbox(text("("+input_mode+")") | color(Color::RGB(255, 255, 50)),com->Render()));
  }) | Maybe(enable.get());
  return Renderer(res, [res, enable, &mode]() {
    *enable = mode != NONE;
    return res->Render();
  });
}


namespace ftxui {

class LogScrollerBase : public ComponentBase {
 public:
  LogScrollerBase(Component child) { Add(child); }
  void jump_to_bottom() {
    if (cursor_box_.y_max < box_.y_max) {
       selected_ += box_.y_max - cursor_box_.y_max;
    }
    size_ = std::max(size_, selected_);
    selected_ = size_;
  }
 private:
  Element OnRender() final {
    auto focused = focus;
    auto style = Focused() ? inverted : nothing;

    Element background = ComponentBase::Render();
    // if (cursor_box_.y_max < box_.y_max) {
    //   selected_ += box_.y_max - cursor_box_.y_max;
    // }
    size_ = std::max(size_, selected_);
    return dbox({
               std::move(background),
               vbox({
                   text(L"") | size(HEIGHT, EQUAL, selected_),
                   text(L"") | style | focused | reflect(cursor_box_),
               }),
           }) |
           vscroll_indicator | yframe | reflect(box_);
  }

  bool OnEvent(Event event) final {
    if (event.is_mouse() && box_.Contain(event.mouse().x, event.mouse().y))
      TakeFocus();
    int selected_old = selected_;
    if (event == Event::ArrowUp || event == Event::Character('k') ||
        (event.is_mouse() && event.mouse().button == Mouse::WheelUp)) {
      if (selected_ > 0)
        selected_--;
    }
    else if ((event == Event::ArrowDown || event == Event::Character('j') ||
         (event.is_mouse() && event.mouse().button == Mouse::WheelDown))) {
      if (cursor_box_.y_max < box_.y_max)
        selected_++;
      size_ = std::max(size_, selected_);
    }
    return selected_old != selected_;
  }


  bool Focusable() const final { return true; }

  int selected_ = 0;
  int size_ = 0;
  Box cursor_box_;
  Box box_;
};

auto LogScroller(Component child) {
  return Make<LogScrollerBase>(std::move(child));
}
}  // namespace ftxui

class ConsoleVisualizer {
  InputMode current_input_mode = NONE;
  cpu::CPU& cpu;
  bool changed;
  Color current_color = Color::RGB(200, 200, 200);
  std::vector<std::vector<std::pair<std::string, Color>>> lines = { {{"", current_color}} };
  public:
  ConsoleVisualizer(cpu::CPU& cpu) : cpu(cpu) {
    auto env = static_cast<cpu::CPU_EnvCall*>(cpu.features["env"].get());
    env->handlers[1] = std::bind(&ConsoleVisualizer::write_int, this, std::placeholders::_1);
    env->handlers[5] = std::bind(&ConsoleVisualizer::read_int, this, std::placeholders::_1);
    env->handlers[11] = std::bind(&ConsoleVisualizer::write_char, this, std::placeholders::_1);
    env->handlers[12] = std::bind(&ConsoleVisualizer::read_char, this, std::placeholders::_1);
    env->handlers[4] = std::bind(&ConsoleVisualizer::write_str, this, std::placeholders::_1);
    env->handlers[8] = std::bind(&ConsoleVisualizer::read_str, this, std::placeholders::_1);
  }
  void write_int(cpu::CPU& cpu) {
    push_str(std::to_string(cpu.state.regs[10]));
    changed = true;
  }
  void write_char(cpu::CPU& cpu) {
    push_char(cpu.state.regs[10]);
    changed = true;
  }
  void read_char(cpu::CPU& cpu) {
    current_input_mode = CHAR;
    cpu.running = false;
  }
  void write_str(cpu::CPU& cpu) {
    try {
      uint64_t i = cpu.state.regs[10];
      while (cpu.state.mem->read(i, 1) != 0) {
        push_char(cpu.state.mem->read(i, 1));
        i++;
      }
    } catch (...) {
    }
    changed = true;
  }
  void read_str(cpu::CPU& cpu) {
    current_input_mode = STRING;
    cpu.running = false;
  }
  void read_int(cpu::CPU& cpu) {
    current_input_mode = INTEGER;
    cpu.running = false;
  }
  void push_char(char c, Color color = Color::RGB(200, 200, 200)) {
    changed = true;
    if (c == '\n') {
      current_color = color;
      lines.emplace_back();
      lines.back().emplace_back("", current_color);
      return;
    }
    if (c == ' ') {
      lines.back().back().first.push_back(' ');
      lines.back().emplace_back("", current_color);
      return;
    }
    if (color != current_color) {
      current_color = color;
      lines.back().emplace_back("", current_color);
    }
    lines.back().back().first.push_back(c);
  }
  void push_str(std::string str, Color color = Color::RGB(200, 200, 200)) {
    for (auto c : str)
      push_char(c, color);
  }
  auto operator()() {
    auto& l = lines;
    auto& ch = changed;
    auto content = ftxui::LogScroller(Renderer([&l]() {
      Elements content;
      for (auto& line : l) {
        Elements line_content;
        for (auto& p : line) {
          line_content.push_back(text(p.first) | color(p.second));
        }
        content.push_back(flexbox(std::move(line_content)) | yframe);
      }
      Element output = vbox(std::move(content));
      return output;
    }));
    auto input = ConsoleInput(current_input_mode, cpu);
    return Renderer(Container::Vertical({content, input}), [content, input, &ch]() {
      if (ch) {
        content->jump_to_bottom();
        ch = false;
      }
      return window(text("Console") | color(Color::RGB(200, 255, 100)),
        vbox({
          content->Render(),
          input->Render(),
        })
      );
    });
  }
 };

}
