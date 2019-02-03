#ifndef CARTRIDGE_H_
#define CARTRIDGE_H_

#include <vector>
#include <cstdint>

namespace mattboy::gameboy::mmu {

  class Cartridge
  {
    public:
      Cartridge(const std::vector<char>& data);
      ~Cartridge();

    private:
      static const uint8_t nintendo_logo_[];
      bool valid_;

  };

}

#endif