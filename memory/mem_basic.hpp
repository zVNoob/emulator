#pragma once

#include "memory.hpp"
#include <vector>

namespace memory {

class RAM : public MemoryRegion {
public:
  std::vector<uint8_t> memory;
  RAM(uint64_t size) : memory(size) {}
  uint8_t read(uint64_t address) override { return memory[address]; }
  void write(uint64_t address, uint8_t value) override {
    memory[address] = value;
  }
};

class ROM : public MemoryRegion {
  std::vector<uint8_t> memory;
public:
  ROM(const std::vector<uint8_t>& data) : memory(data.begin(), data.end()) {}
  uint8_t read(uint64_t address) override { return memory[address]; }
  void write(uint64_t address, uint8_t value) override {
    throw MemoryAccessViolation(address);
  }
};

}
