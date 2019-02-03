#include "mmu.hpp"

#include <iostream>

namespace mattboy::gameboy::mmu {

  MMU::MMU() : cart_(nullptr), memory_ {0}
  {

  }

  MMU::~MMU()
  {

  }

  std::shared_ptr<Cartridge> MMU::GetCartridge()
  {
    return cart_;
  }

  void MMU::LoadCartridge(const std::vector<char>& data)
  {
    cart_ = std::shared_ptr<Cartridge>(new Cartridge(data));
  }

  void MMU::Reset()
  {
    memory_[0xFF05] = 0x00;
    memory_[0xFF06] = 0x00;
    memory_[0xFF07] = 0x00;
    memory_[0xFF10] = 0x80;
    memory_[0xFF11] = 0xBF;
    memory_[0xFF12] = 0xF3;
    memory_[0xFF14] = 0xBF;
    memory_[0xFF16] = 0x3F;
    memory_[0xFF17] = 0x00;
    memory_[0xFF19] = 0xBF;
    memory_[0xFF1A] = 0x7F;
    memory_[0xFF1B] = 0xFF;
    memory_[0xFF1C] = 0x9F;
    memory_[0xFF1E] = 0xBF;
    memory_[0xFF20] = 0xFF;
    memory_[0xFF21] = 0x00;
    memory_[0xFF22] = 0x00;
    memory_[0xFF23] = 0xBF;
    memory_[0xFF24] = 0x77;
    memory_[0xFF25] = 0xF3;
    memory_[0xFF26] = 0xF1;
    memory_[0xFF40] = 0x91;
    memory_[0xFF42] = 0x00;
    memory_[0xFF43] = 0x00;
    memory_[0xFF45] = 0x00;
    memory_[0xFF47] = 0xFC;
    memory_[0xFF48] = 0xFF;
    memory_[0xFF49] = 0xFF;
    memory_[0xFF4A] = 0x00;
    memory_[0xFF4B] = 0x00;
    memory_[0xFFFF] = 0x00;

    // TODO: Assign values from cartridge...
  }

}