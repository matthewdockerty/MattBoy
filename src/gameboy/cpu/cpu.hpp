#ifndef CPU_H_
#define CPU_H_

#include <cstdint>
#include "../mmu/mmu.hpp"

#define REG_A registers_.a_
#define REG_F registers_.f_
#define REG_B registers_.b_
#define REG_C registers_.c_
#define REG_D registers_.d_
#define REG_E registers_.e_
#define REG_H registers_.h_
#define REG_L registers_.l_

#define REG_AF REG_A
#define REG_BC REG_B
#define REG_DE REG_D
#define REG_HL REG_H

namespace mattboy::gameboy::cpu {

  class CPU
  {
    public:
      CPU();
      ~CPU();

      void Reset();
      void Cycle(mmu::MMU& mmu);

    private:
      struct
      {
        uint8_t a_, f_;
        uint8_t b_, c_;
        uint8_t d_, e_;
        uint8_t h_, l_;
      } registers_;

      uint16_t sp_;
      uint16_t pc_;

      static const int CPU_SPEED_CLOCKS_HZ = 4194304;
      static constexpr float NANOSECONDS_PER_CLOCK = 1000000000.0 / CPU_SPEED_CLOCKS_HZ; 

      inline void RegisterSet(uint8_t& reg, uint8_t value) { reg = value; };
      inline void RegisterSetPair(uint8_t& first, uint16_t value)
      {
        first = (value << 8) & 0xFF;
        *((&first) + 1) =  value & 0xFF;
      }
  };

}

#endif