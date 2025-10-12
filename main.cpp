#include <iomanip>
#include <iostream>
#include <sstream>
#include "memory/mem_basic.hpp"
#include "reader/assembler.hpp"
#include "cpu/cpu_all_feature.hpp"
#include "visualizer/visualizer.hpp"

std::string asm_text = "\
.text\n\
li s0, 1\n\
li t0, 0\n\
li t1 ,5\n\
loop:\n\
add s0, s0, s0\n\
add s0, s0, t0\n\
addi t0,t0, 1\n\
blt t0, t1, loop\n\
";

// std::string asm_text = ".data\n\
// a: .word 1\n\
// b: .byte 2\n\
// ";

using namespace std;

int main() {
  std::stringstream ss;
  ss << asm_text;
  reader::assembler a(ss);
  auto temp = a();
  auto mem = std::make_shared<memory::Memory>();
  mem->add_region(0x00400000, temp[0].size, std::make_shared<memory::ROM>(temp[0].data));
  mem->add_region(0x10010000, 1000, std::make_shared<memory::RAM>(1000));
  for (int i = 0; i < temp[0].size; i+=4) {
    std::cout << std::hex << std::setfill('0') << std::setw(8) << mem->read(0x00400000+i, 4) << "\n";
  }
  getchar();
  auto cpu = cpu::all_feature();
  cpu.state.mem = mem;
  cpu.state.pc = 0x00400000;
  visualizer::Visualizer v(cpu);
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
