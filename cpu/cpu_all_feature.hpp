#pragma once

#include "cpu.hpp"
#include <memory>

#include "cpu_32I.hpp"
#include "cpu_envcall.hpp"

namespace cpu {
CPU all_feature() {
  CPU cpu;
  cpu.features["32I"] = std::make_unique<CPU_32I>();
  cpu.features["env"] = std::make_unique<CPU_EnvCall>();
  return cpu;
}
}
