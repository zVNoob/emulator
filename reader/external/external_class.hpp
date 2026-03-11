#pragma once
#include "process.hpp"
#include "../reader.hpp"
#include <filesystem>
#include <fstream>
#include <string>

namespace reader {
using namespace _external;
class external : public Reader {
  std::string _linkerscript = "\
MEMORY {\n\
  rom (rx) : ORIGIN = 0x00400000, LENGTH = 0x10000\n\
  ram (rwx) : ORIGIN = 0x10010000, LENGTH = 0x10000\n\
}\n\
SECTIONS {\n\
  .text : { *(.text) } > rom\n\
  .data : { *(.data) } > ram\n\
}\n\
";
class AssemblingError : public std::runtime_error {
public:
  AssemblingError(std::string msg) : std::runtime_error(msg) {}
};
  std::string new_filename = "";
public:
  external(std::filesystem::path filename) {
    if (filename.extension() == ".asm") {
      {
      auto temp = Process("riscv64-unknown-linux-gnu-as",{"-march=rv64i",filename, "-o", filename.parent_path() / ( "." + filename.stem().string() + ".o")});
      std::string err;
      std::string t;
      while ((t = temp.readline(true)) != "") err += t;
      if (!err.empty()) throw AssemblingError(err);
      }
      {
      std::ofstream out(".ld");
      out << _linkerscript;
      out.close();
      srand(time(NULL));
      new_filename = "." + std::to_string(rand() % 10000) + ".elf";
      Process("riscv64-unknown-linux-gnu-ld",{"-T", ".ld", "-o", new_filename, filename.parent_path() / ( "." + filename.stem().string() + ".o")}).wait();
      }
      {
        remove(".ld");
        remove(filename.parent_path() / ( "." + filename.stem().string() + ".o"));
      }
    }
  }
  ~external() {
    remove(new_filename.c_str());
  }
};
};
