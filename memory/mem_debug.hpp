#pragma once

#include "memory.hpp"
#include <vector>

namespace memory {

class MemModifyNotifier : public MemoryRegion {
public:
  std::vector<uint64_t> modifications;
  std::shared_ptr<MemoryRegion> memory;
  uint8_t read(uint64_t address) override {
    return memory->read(address);
  }
  void write(uint64_t address, uint8_t value) override {
    modifications.push_back(address);
    memory->write(address, value);
  }
  std::vector<uint64_t> get_modifications() {
    auto temp = std::move(modifications);
    modifications.clear();
    return temp;
  }
};

}

