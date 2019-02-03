#ifndef CARTRIDGE_H_
#define CARTRIDGE_H_

#include <vector>
#include <cstdint>
#include <string>

#define NINTENDO_LOGO_SIZE 48
#define NINTENDO_LOGO_LOCATION 0x104
#define TITLE_LOCATION 0x134
#define TITLE_SIZE 16

namespace mattboy::gameboy::mmu {

  class Cartridge
  {
    public:
      Cartridge(const std::vector<char>& data);
      ~Cartridge();

      const std::string& GetTitle();

    private:
      static const uint8_t nintendo_logo_[NINTENDO_LOGO_SIZE];
      bool valid_;
      std::string title_;
  };

}

#endif