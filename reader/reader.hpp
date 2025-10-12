#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace reader {
  struct Read_Data {
    uint64_t start;
    uint64_t size;
    std::vector<uint8_t> data;
  };
  typedef std::vector<Read_Data> Region_Map;
  class Reader {
  public:
    virtual Region_Map operator()() = 0;
  };
}
