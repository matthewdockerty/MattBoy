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

#define FLAG_ZERO 7
#define FLAG_ADD_SUB 6
#define FLAG_HALF_CARRY 5
#define FLAG_CARRY 4
#define FLAG_UNUSED_3 3
#define FLAG_UNUSED_2 2
#define FLAG_UNUSED_1 1
#define FLAG_UNUSED_0 0

namespace mattboy::gameboy::cpu {

  class CPU
  {
    public:
      CPU();
      ~CPU();

      void Reset();
      int Cycle(mmu::MMU& mmu);

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

      inline void SetRegister(uint8_t& reg, uint8_t value) { reg = value; };
      inline void SetRegisterPair(uint8_t& first, uint8_t& second, uint16_t value)
      {
        first = (uint8_t) (value >> 8) & 0xFF;
        second = (uint8_t) value & 0xFF;
      }

      inline uint16_t GetRegisterPair(uint8_t& first, uint8_t& second)
      {
        return (uint16_t) ((first << 8) | second);
      }

      inline void SetFlag(int flag, bool value)
      {
        if (value)
          REG_F |= 1UL << flag;
        else
          REG_F &= ~(1UL << flag);
      }

      inline bool CheckFlag(int flag)
      {
        return (REG_F >> flag) & 1U;
      }

      inline void PushStack(mmu::MMU& mmu, uint16_t value)
      {
        mmu.WriteByte(--sp_, (value >> 8) & 0xFF);
        mmu.WriteByte(--sp_, value & 0xFF);
      }

  };

}

#endif