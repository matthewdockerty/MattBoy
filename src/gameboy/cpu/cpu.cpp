#include "cpu.hpp"

#include <iostream>

namespace mattboy::gameboy::cpu {

  CPU::CPU()
  {

  }

  CPU::~CPU()
  {

  }

  void CPU::Reset()
  {
    RegisterSet(REG_A, 0x01);
    RegisterSet(REG_F, 0xB0);
    RegisterSetPair(REG_BC, 0x0013);
    RegisterSetPair(REG_DE, 0x00D8);
    RegisterSetPair(REG_HL, 0x014D);
    sp_ = 0xFFFE;
    pc_ = 0x100;
  }

  void CPU::Cycle(mmu::MMU& mmu)
  {
    uint8_t instruction = mmu.ReadByte(pc_);
    pc_++;
    std::cout << instruction << std::endl;
    
  }

}