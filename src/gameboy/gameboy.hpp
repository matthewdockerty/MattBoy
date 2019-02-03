#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include "mmu/mmu.hpp"
#include "cpu/cpu.hpp"

namespace mattboy::gameboy {

  class Gameboy
  {
    public:
      Gameboy();
      ~Gameboy();

      void Reset();
      void LoadCartridge(const std::vector<char>& data);

    private:
      mmu::MMU mmu_;
      cpu::CPU cpu_;
  };

}

#endif