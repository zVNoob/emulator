#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <iomanip>
namespace memory {

class MemoryAccessViolation : public std::runtime_error {
public:
  std::string to_hex(uint64_t address) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(16) << address;
    return ss.str();
  }
  MemoryAccessViolation(uint64_t address) : std::runtime_error("Memory access violation at address " + to_hex(address)) {}
};
class MemoryRegionOverlap : public std::exception {};

class MemoryRegion {
public:
  virtual uint8_t read(uint64_t address) = 0;
  virtual void write(uint64_t address, uint8_t value) = 0;
};

class Memory {
private:
  std::map<uint64_t, std::pair<uint64_t, std::shared_ptr<MemoryRegion>>> regions;
public:
  uint64_t read(uint64_t address, uint8_t int_size) {
    auto it = regions.upper_bound(address);
    if (it == regions.begin()) {
      throw MemoryAccessViolation(address);
    }
    --it;
    if (it->second.first < address + int_size) {
      throw MemoryAccessViolation(address);
    }
    // convert int_size bytes to local endian format
    //
    uint64_t value = 0;
    for (int i = 0; i < int_size; i++) {
      value <<= 8;
      value |= it->second.second->read(address - it->first + i);
    }
    return value;
  }
  void write(uint64_t address, uint64_t value, uint8_t int_size) {
    auto it = regions.upper_bound(address);
    if (it == regions.begin()) {
      throw MemoryAccessViolation(address);
    }
    --it;
    if (it->second.first < address) {
      throw MemoryAccessViolation(address);
    }
    // convert int_size bytes to local endian format
    //
    for (int i = int_size - 1; i >= 0; i--) {
      it->second.second->write(address - it->first + i, (value >> ((int_size - i - 1) * 8)) & 0xFF);
    }
  }
  void add_region(uint64_t address, uint64_t size, std::shared_ptr<MemoryRegion> region) {
    auto it = regions.upper_bound(address);
    if (it != regions.end()) {
      if (it->first < address + size) {
        throw MemoryRegionOverlap();
      }
    }
    if (it != regions.begin()) {
      --it;
      if (it->second.first > address) {
        throw MemoryRegionOverlap();
      }
    }
    regions[address] = std::make_pair(address + size, region);
  }
};

}
