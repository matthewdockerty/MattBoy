#include "gameboy.hpp"

namespace mattboy::gameboy {
    using namespace mmu;

    Gameboy::Gameboy()
    {
        
    }

    Gameboy::~Gameboy()
    {
    }

    void Gameboy::Reset()
    {
      mmu_.Reset();
      cpu_.Reset();
    }

    void Gameboy::LoadCartridge(const std::vector<char>& data)
    {
      mmu_.LoadCartridge(data);
      Reset();
    }

    std::shared_ptr<mmu::Cartridge> Gameboy::GetCartridge()
    {
      return mmu_.GetCartridge();
    }

} 