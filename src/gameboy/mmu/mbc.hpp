#ifndef MBC_H_
#define MBC_H_

#include <cstdint>
#include "cartridge_type.hpp"

namespace mattboy::gameboy::mmu {

  class MBC
  {
    public:
      MBC(CartridgeType type, int rom_size, int ram_size, bool battery) : type_(type), rom_size_(rom_size), ram_size_(ram_size), battery_(battery) {}
      virtual ~MBC() {}

      virtual uint8_t* GetRomBank0() = 0;
      virtual uint8_t* GetROMBankN(int n) = 0;
      virtual uint8_t* GetRAMBankN(int n) = 0;

      const static int ROM_BANK_SIZE = 16 * 1024;

    protected:
      CartridgeType type_;
      int rom_size_;
      int ram_size_;
      bool battery_;
  };

}

#endif