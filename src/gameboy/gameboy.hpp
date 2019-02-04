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
      void LoadCartridge(const std::vector<char>& data, const std::string& rom_file);
      std::shared_ptr<mmu::Cartridge> GetCartridge();

      void Cycle();

    private:
      mmu::MMU mmu_;
      cpu::CPU cpu_;
  };

}

#endif