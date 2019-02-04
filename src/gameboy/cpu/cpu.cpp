#include "cpu.hpp"

#include <iostream>
#include <chrono>

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
    int cycles = 0; // CPU cycle count used for instruction (measured in CLOCK CYCLES, with a 4.19MHz clockspeed)
    const auto start_time = std::chrono::high_resolution_clock::now();

    uint8_t instruction = mmu.ReadByte(pc_++);
    printf("instruction: %02x   pc: %04x\n", instruction, pc_);
    
    switch (instruction)
    {
      case 0x00: // No op
        cycles += 4;
        break;
      case 0xC3: // Jump to a memory address
        cycles += 16;
        pc_ = mmu.Read2Bytes(pc_);
        break;
      default:
        printf("unimplemented opcode: 0x%02x\n", instruction);
        for (; ;);
        break;
    }

    // CPU cycle timing
    auto elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
    long long nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time).count();
    while (nanoseconds < cycles * NANOSECONDS_PER_CLOCK)
    {
      elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
      nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time).count();
    }

  }

}