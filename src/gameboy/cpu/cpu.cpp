#include "cpu.hpp"

#include <iostream>
// #include <chrono>

namespace mattboy::gameboy::cpu {

  CPU::CPU()
  {

  }

  CPU::~CPU()
  {

  }

  void CPU::Reset()
  {
    SetRegister(REG_A, 0x01);
    SetRegister(REG_F, 0xB0);
    SetRegisterPair(REG_B, REG_C, 0x0013);
    SetRegisterPair(REG_D, REG_E, 0x00D8);
    SetRegisterPair(REG_H, REG_L, 0x014D);
    sp_ = 0xFFFE;
    pc_ = 0x100;
  }

  int CPU::Cycle(mmu::MMU& mmu)
  {
    int cycles = 0; // CPU cycle count used for instruction (measured in CLOCK CYCLES, with a 4.19MHz clockspeed)
    // const auto start_time = std::chrono::high_resolution_clock::now();

    uint8_t instruction = mmu.ReadByte(pc_);
    printf("instruction: %02x   pc: %04x\n", instruction, pc_);
    pc_++;

    switch (instruction)
    {
      case 0x00: // NOP
        cycles += 4;
        break;

      case 0x05: // DEC B
        cycles += 4;
        REG_B--;
        SetFlag(FLAG_ZERO, REG_B == 0);
        SetFlag(FLAG_ADD_SUB, true);
        SetFlag(FLAG_HALF_CARRY, (REG_B & 0x0F) == 0x0F);
        break;

      case 0x06: // LD B n
        cycles += 8;
        SetRegister(REG_B, mmu.ReadByte(pc_++));
        break;

      case 0x0D: // DEC C
        cycles += 4;
        REG_C--;
        SetFlag(FLAG_ZERO, REG_C == 0);
        SetFlag(FLAG_ADD_SUB, true);
        SetFlag(FLAG_HALF_CARRY, (REG_C & 0x0F) == 0x0F);
      break;

      case 0x0e: // LD C n
        cycles += 8;
        SetRegister(REG_C, mmu.ReadByte(pc_++));
        break;

      case 0x20: // JR NZ,n
        if (!CheckFlag(FLAG_ZERO))
        {
          cycles += 12;
          pc_ += (int8_t) mmu.ReadByte(pc_) + 1;
        }
        else
        {
          cycles += 8;
          pc_ ++;
        }
        break;
      case 0x21: // LD HL nn
        cycles += 12;
        SetRegisterPair(REG_H, REG_L, mmu.Read2Bytes(pc_));
        pc_ += 2;
        break;

      case 0x32: // LD (HL-),A
      {
        cycles += 8;
        uint16_t hl = GetRegisterPair(REG_H, REG_L);
        mmu.WriteByte(hl, REG_A);
        SetRegisterPair(REG_H, REG_L, hl - 1);
        break;
      }
      case 0x3E: // LD A,#
        cycles += 8;
        SetRegister(REG_A, mmu.ReadByte(pc_++));
        break;

      case 0xAF: // XOR A
        cycles += 4;
        REG_A ^= REG_A;
        SetFlag(FLAG_ZERO, REG_A == 0);
        SetFlag(FLAG_ADD_SUB, false);
        SetFlag(FLAG_HALF_CARRY, false);
        SetFlag(FLAG_CARRY, false);
        break;

      case 0xC3: // Jump to a memory address
        cycles += 16;
        pc_ = mmu.Read2Bytes(pc_);
        break;

      case 0xE0: // LD ($FF00+n),A
        cycles += 12;
        mmu.WriteByte((uint8_t)0xFF00 + mmu.ReadByte(pc_++), REG_A);
        break;

      case 0xF0: // LD A,($FF00+n)
        cycles += 12;
        SetRegister(REG_A, mmu.ReadByte(0xFF00 + mmu.ReadByte(pc_++)));
        break;

      case 0xF3: // DI
        cycles += 4;
        std::cout << "TODO: Disable interrupts" << std::endl;
        break;

      case 0xFE: // CP n
      {
        cycles += 8;
        uint8_t n = mmu.ReadByte(pc_++);
        uint8_t result = REG_A - n;
        SetFlag(FLAG_ZERO, result == 0);
        SetFlag(FLAG_ADD_SUB, true);
        SetFlag(FLAG_HALF_CARRY, (REG_A & 0xF) - (n & 0xF) < 0);
        SetFlag(FLAG_CARRY, REG_A < n);
      }
        break;

      default:
        printf("unimplemented opcode: 0x%02x\n", instruction);
        for (; ;);
        break;
    }

    // // CPU cycle timing
    // auto elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
    // long long nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time).count();
    // while (nanoseconds < cycles * NANOSECONDS_PER_CLOCK)
    // {
    //   elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
    //   nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time).count();
    // }
    return cycles;
  }

}