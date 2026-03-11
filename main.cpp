#include <iomanip>
#include <iostream>
#include <sstream>
#include <stack>
#include "memory/mem_basic.hpp"
#include "reader/assembler.hpp"
#include "cpu/cpu_all_feature.hpp"
#include "reader/assembler/calculator.hpp"
#include "visualizer/visualizer.hpp"

std::string asm_text = "\
.text\n\
addi a1, zero, (3*(3+3)*(3-1)*3)+2\n\
add a0, a1, a1\n\
addi a7, zero, 1\n\
ecall\n\
add a0, a1, a0\n\
ecall\n\
addi a7, zero, 10\n\
ecall\n\
";

// std::string asm_text = ".data\n\
// a: .word 1\n\
// b: .byte 2\n\
// ";

using namespace std;

int main() {
  std::map<std::string, uint64_t> registers;
  _assembler::calculator calc(registers);
  //cout << calc("(3*(3+3)*(3-1)*3)+2") << endl;
  std::stringstream ss;
  ss << asm_text;
  reader::assembler a(ss);
  auto temp = a();
  auto mem = std::make_shared<memory::Memory>();
  for (auto& i : temp) {
    int count = 1;
    int size = i.size;
    while (size) {
      size >>= 1;
      count <<= 1;
    }
    auto mem_reg = std::make_shared<memory::RAM>(count);
    mem->add_region(i.start, i.size, mem_reg);
    for (int j = 0; j < i.size; j++) {
      mem_reg->memory[j] = i.data[j];
    }
  }
  uint64_t stack_size = 0x1000;
  mem->add_region(0x7fffffff - stack_size, stack_size, std::make_shared<memory::RAM>(stack_size));
  mem->add_region(0x10010000, 1000, std::make_shared<memory::RAM>(1000));
  // for (int i = 0; i < temp[0].size; i+=4) {
  //   std::cout << std::hex << std::setfill('0') << std::setw(8) << mem->read(0x00400000+i, 4) << "\n";
  // }
  // getchar();
  auto cpu = cpu::all_feature();
  cpu.state.mem = mem;
  cpu.state.pc = 0x00400000;
  cpu.state.regs[2] = 0x7fffffff - stack_size;
  visualizer::Visualizer(cpu, 0x00400000, temp[0].size);
  // while (1) {
  //   std::cout << "\x1b[2J\x1b[H";
  //   std::cout << "pc=" << std::hex << std::setfill('0') << std::setw(8) << cpu.state.pc << "\n";
  //   for (int i = 0; i < 32; i++) {
  //     std::cout << "x" << std::dec << i << "=" << std::hex << std::setfill('0') << std::setw(8) << cpu.state.regs[i] << "\n";
  //   }
  //   getchar();
  //   cpu.step();
  // }
  return 0;
}
