#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include "mmu/mmu.hpp"
#include "cpu/cpu.hpp"
#include "gpu/gpu.hpp"

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
      void SetRunning(bool value);

    private:
      mmu::MMU mmu_;
      cpu::CPU cpu_;
      gpu::GPU gpu_;

      bool running_;
  };

}

#endif