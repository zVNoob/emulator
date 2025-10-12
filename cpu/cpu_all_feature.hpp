#pragma once

#include "cpu.hpp"
#include <memory>

#include "cpu_I.hpp"

namespace cpu {
CPU all_feature() {
  CPU cpu;
  cpu.features["32I"] = std::make_unique<CPU_I>(); 
  return cpu;
}
}
