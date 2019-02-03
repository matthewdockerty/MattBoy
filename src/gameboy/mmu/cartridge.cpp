#include "cartridge.hpp"

#include <iostream>

namespace mattboy::gameboy::mmu {

  const uint8_t Cartridge::nintendo_logo_[] = { 
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
  };

  Cartridge::Cartridge(const std::vector<char>& data) : valid_(true)
  {
    for (int i = 0; i < NINTENDO_LOGO_SIZE; i++)
    {
      if(static_cast<uint8_t>(data[NINTENDO_LOGO_LOCATION + i]) != nintendo_logo_[i])
      {
        valid_ = false;
        break;
      }
    }

    title_ = std::string(&data[TITLE_LOCATION], &data[TITLE_LOCATION] + TITLE_SIZE);
    
    std::cout << "VALID: " << valid_ << std::endl;
  }

  Cartridge::~Cartridge()
  {

  }

  const std::string& Cartridge::GetTitle()
  {
    return title_;
  }

}
