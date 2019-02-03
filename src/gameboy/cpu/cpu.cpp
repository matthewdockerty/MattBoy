#include "cpu.hpp"

#include <iostream>

namespace mattboy::gameboy::cpu {

  CPU::CPU()
  {
    Reset();

    std::printf("af: 0x%04x   b: 0x%02x   c: 0x%02x   d: 0x%02x   e: 0x%02x   h: 0x%02x   l: 0x%02x   pc: 0x%04x   sp: 0x%04x\n", *reinterpret_cast<uint16_t*>(&registers_.a_), registers_.b_, registers_.c_, registers_.d_, registers_.e_, registers_.h_, registers_.l_, sp_, pc_);
    
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

}