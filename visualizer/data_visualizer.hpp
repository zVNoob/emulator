#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <bits/stdc++.h>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>
#include <memory>
#include <sstream>
#include <cctype>
#include "../cpu/cpu.hpp"

namespace visualizer {
using namespace ftxui;

static ButtonOption TextDataStyle() {
  ButtonOption option;
  option.transform = [](const EntryState& s) {
    auto res = text(s.label);
    if (s.focused)
      res |= bgcolor(Color::RGB(50, 50, 50));
    return res;
  };
  return option;
}

enum data_type {
    HEX,
    DEC,
    WCHAR,
    BIN,
};

static std::string to_utf8(uint64_t codepoint) {
    std::string utf8_string;
    if (codepoint <= 0x7F) { // 1-byte sequence (ASCII)
        utf8_string += static_cast<char>(codepoint);
    } else if (codepoint <= 0x7FF) { // 2-byte sequence
        utf8_string += static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F));
        utf8_string += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0xFFFF) { // 3-byte sequence
        utf8_string += static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F));
        utf8_string += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        utf8_string += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0x10FFFF) { // 4-byte sequence
        utf8_string += static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07));
        utf8_string += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
        utf8_string += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        utf8_string += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else utf8_string = "�";
    return utf8_string;
}

static std::string transform_char(uint64_t c) {
  if (c < 256) {
    if (c == '\\') return "\\\\";
    if (std::isprint(c)) return std::string(1, c);
    if (c == '\n') return "\\n";
    if (c == '\t') return "\\t";
    if (c == '\r') return "\\r";
    if (c == '\0') return "\\0";
    if (c == '\a') return "\\a";
    if (c == '\b') return "\\b";
    if (c == '\v') return "\\v";
    if (c == '\f') return "\\f";
    if (c == '\e') return "\\e";
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(2) << c;
    return "\\x" + ss.str();
  }
  return to_utf8(c);
}

static ButtonOption DataStyle() {
  ButtonOption option;
  option.transform = [](const EntryState& s) {
    auto res = text(s.label);
    if (s.focused)
      res |= color(Color::RGB(200, 255, 100));
    return res;
  };
  return option;
}

static auto SingleDataVisualizer(cpu::CPU& cpu, data_type* type,
                                 uint64_t* start, uint64_t offset, uint64_t* len) {
  auto s = std::make_shared<std::string>();
  auto avail = std::make_shared<bool>(1);
  auto b = Button(s.get(),[](){}, DataStyle()) | Maybe(avail.get());
  return Renderer(b,[&cpu, start, offset, len,type,s,b, avail]() {
    uint64_t value;
    try {
      value = cpu.state.mem->read(*start + offset * *len, *len);
      *avail = 1;
    } catch (...) {
      *avail = 0;
      return text("");
    }
    std::stringstream ss;
    switch (*type) {
      case HEX:
        ss << std::hex << std::setfill('0') << std::setw(*len * 2) << value;
      break;
      case DEC:
      {
        int64_t val = *(int64_t*)&value;
        ss << val;
      }
      break;
      case WCHAR:
        ss << transform_char(value);
      break;
      case BIN:
      {
          for (int i = 0; i < *len * 8; i++) {
            ss << ((value >> ((*len * 8) - i)) & 1);
          }
      }
      break;
    }
    *s = ss.str();
    return b->Render();
  });
}

static int64_t to_int_with_base(const std::string& s, int base) {
    uint64_t ret = 0;
    for (char c : s) {
      if (c >= '0' && c <= '9') ret = ret * base + (c - '0');
      else if (c >= 'a' && c <= 'f') ret = ret * base + (c - 'a' + 10);
      else if (c >= 'A' && c <= 'F') ret = ret * base + (c - 'A' + 10);
    }
    return *(int64_t*)&ret;
  }
static int64_t to_int(const std::string& s) {
    if (s[0] == '0' && s[1] == 'x') return to_int_with_base(s.substr(2), 16);
    if (s[0] == '0' && s[1] == 'b') return to_int_with_base(s.substr(2), 2);
    if (s[0] == '0' && s[1] == 'o') return to_int_with_base(s.substr(2), 8);
    return to_int_with_base(s, 10);
}

static std::string to_hex(uint64_t value) {
  std::stringstream ss;
  ss << std::hex << std::setfill('0') << std::setw(8) << value;
  return ss.str();
}

static std::string to_dec(uint64_t value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

static InputOption HeaderInputStyle() {
  InputOption res = InputOption::Default();
  res.transform = [](const InputState& s) {
    if (s.focused) {
      return s.element | bgcolor(Color::RGB(150, 100, 200));
    } 
    if (s.hovered) {
      return s.element | bgcolor(Color::RGB(50, 50, 50));
    }

    return s.element;
  };
  res.multiline = false;
  return res;
}

static auto SingleInput(uint64_t* value, bool is_hex, int max_len = -1) {
  auto s = std::make_shared<std::string>((is_hex)?to_hex(*value):to_dec(*value));
  auto inp = Input(s.get(), HeaderInputStyle());
  inp |= CatchEvent([s,is_hex,max_len] (Event ev) {
    if (ev.is_character()) {
      if (max_len > 0) if (s->size() >= max_len) return true;
      if (is_hex) {
        if (ev.character()[0] >= 'a' && ev.character()[0] <= 'f')
          return false;
        if (ev.character()[0] >= 'A' && ev.character()[0] <= 'F')
          return false;
      }
      if (ev.character()[0] >= '0' && ev.character()[0] <= '9')
        return false;
      return true;
    }
    return false;
  });
  return Renderer(inp,[inp,s,value,is_hex]() {
    if (is_hex) *value = to_int_with_base(*s, 16);
    else *value = to_int_with_base(*s,10);
    return inp->Render();
  });
}

static MenuOption DataMenuStyle() {
  MenuOption option;
  option.direction = Direction::Right;
  option.entries_option.transform = [](const EntryState& state) {
    Element e = text(state.label);
    if (state.focused)
      e |= bgcolor(Color::RGB(50, 50, 50));
    if (state.active) {
      e |= color(Color::RGB(200, 255, 100));
    } else e |= color(Color::RGB(150, 150, 150));
    return e;
  };
  option.elements_infix = [] { return text(" "); };
 
  return option;
}

static auto TypeChoice(data_type* type) {
  auto temp = std::make_shared<std::vector<std::string>>();
  *temp = {
    "hex",
    "dec",
    "char",
    "bin",
  };
  auto choice = std::make_shared<int>(0);
  auto d = Menu(temp.get(),choice.get(), DataMenuStyle());
  return Renderer(d,[d,temp, choice, type]() {
    if (*choice == 0) *type = data_type::HEX;
    if (*choice == 1) *type = data_type::DEC;
    if (*choice == 2) *type = data_type::WCHAR;
    if (*choice == 3) *type = data_type::BIN;
    return d->Render();
  }); 
}

static auto DataHeader(uint64_t* start, uint64_t* len, uint64_t* int_len, data_type* type, int* is_focusing, uint64_t* focusing) {
  std::vector<Component> res;
  res.push_back(SingleInput(start, true));
  res.push_back(SingleInput(len, false));
  res.push_back(SingleInput(int_len, false, 1));
  res.push_back(TypeChoice(type));
  return Renderer(Container::Horizontal(res),[res,is_focusing,focusing]() {
    auto elem = hbox({
        text("0x") | color(Color::RGB(255, 255, 255)),
        res[0]->Render() | color(Color::RGB(255, 255, 255)),
        text(" + "),
        res[1]->Render() | color(Color::RGB(255, 255, 255)),
        text(" * "),
        res[2]->Render() | color(Color::RGB(255, 255, 255)),
        text(" "),
        res[3]->Render(),
        
      });
    if (*is_focusing) {
      std::stringstream ss;ss << std::hex << std::setfill('0') << std::setw(8) << *focusing;
      elem = text("0x" + ss.str()) | color(Color::RGB(255, 255, 50)) | bold;
    }
    return hbox({
      text("Data - ") | color(Color::RGB(200, 255, 100)),
      elem
    });
  });
}

class DataVisualizer {
public:
  uint64_t start;
  uint64_t int_len;
  Component data;
  cpu::CPU& cpu;
  uint64_t focusing;
  int is_focusing;
  data_type type;
  uint64_t len;
  DataVisualizer(cpu::CPU& cpu, uint64_t start, uint int_len) :
    cpu(cpu) {
    this->start = start;
    this->int_len = int_len;
    this->focusing = start;
    this->is_focusing = 0;
    this->len = 100;
    this->data = Container::Flex({}, nullptr, FlexboxConfig().SetGap(1, 0));
    for (uint i = 0; i < 100; i++) {
      data->Add(SingleDataVisualizer(cpu, &type, &this->start, i, &this->int_len));
    }
  }
  auto operator()() {
    auto& d = data;
    auto header = DataHeader(&start, &len, &int_len, &type, &is_focusing, &focusing);
    auto& l = len;
    auto& s = start;
    auto& il = int_len;
    auto& f = focusing;
    auto& i_f = is_focusing;
    auto& cp = cpu;
    auto& ty = type;
    return Renderer(Container::Vertical({header, data}),[&cp,&d,header,&l,&s,&il,&f,&i_f,&ty]() {
      int old_len = d->ChildCount();
      if (l > old_len) {
        for (int i = old_len; i < l; i++) {
          d->Add(SingleDataVisualizer(cp, &ty, &s, i, &il));
        }
      }
      if (l < old_len) {
        for (int i = old_len; i > l; i--) {
          d->ChildAt(i - 1)->Detach();
        }
      }
      if ((i_f = d->Focused())) f = s + d->ActiveChild()->Index() * il;
      return window(header->Render(), d->Render() | vscroll_indicator | yframe);
    });
  }
};
}

