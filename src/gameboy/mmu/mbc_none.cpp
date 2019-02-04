#include "mbc_none.hpp"

#include <iostream>

namespace mattboy::gameboy::mmu {

  MBCNone::MBCNone(CartridgeType type, int rom_size, int ram_size, bool battery, const std::vector<char>& data) : MBC(type, rom_size, ram_size, battery)
  {
    ram_ = new uint8_t[RAM_SIZE];

    memcpy(rom_bank_0_, &data[0], ROM_BANK_SIZE);
    memcpy(rom_bank_1_, &data[ROM_BANK_SIZE], ROM_BANK_SIZE);
    // TODO: Load battery backed RAM from saved file
  }

  MBCNone::~MBCNone()
  {
    delete ram_;
  }

  uint8_t* MBCNone::GetRomBank0()
  {
    return rom_bank_0_;
  }

  uint8_t* MBCNone::GetROMBankN(int n)
  {
    return n == 0 ? rom_bank_0_ : n == 1 ? rom_bank_1_ : nullptr;
  }

  uint8_t* MBCNone::GetRAMBankN(int n)
  {
    if (type_ == ROM_RAM || type_ == ROM_RAM_BATTERY)
      return ram_;

    return nullptr;
  }

}
