#include "gameboy.hpp"

namespace mattboy::gameboy {
    using namespace mmu;

    Gameboy::Gameboy() : running_(false)
    {
        
    }

    Gameboy::~Gameboy()
    {
    }

    void Gameboy::Reset()
    {
      mmu_.Reset();
      cpu_.Reset();
      gpu_.Reset();
    }

    void Gameboy::LoadCartridge(const std::vector<char>& data, const std::string& rom_file)
    {
      mmu_.LoadCartridge(data, rom_file);
      if (mmu_.GetCartridge()->IsValid())
        Reset();
    }

    std::shared_ptr<mmu::Cartridge> Gameboy::GetCartridge()
    {
      return mmu_.GetCartridge();
    }

    void Gameboy::SetRunning(bool value)
    {
      running_ = value;
    }

    void Gameboy::Cycle()
    {
      if (running_ && mmu_.GetCartridge() != nullptr && mmu_.GetCartridge()->IsValid())
      {
        int cpu_cycles = cpu_.Cycle(mmu_);
        int gpu_cycles = cpu_cycles / 2;
        
        gpu_.Cycle(gpu_cycles, mmu_);
      }
    }

} 