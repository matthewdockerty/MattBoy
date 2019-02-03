#ifndef CARTRIDGE_H_
#define CARTRIDGE_H_

#include <vector>
#include <cstdint>
#include <string>

#define NINTENDO_LOGO_SIZE 48
#define NINTENDO_LOGO_LOCATION 0x104
#define TITLE_LOCATION 0x134
#define TITLE_SIZE 16
#define CGB_FLAG_LOCATION 0x143 
#define CGB_FLAG_VALUE 0xC0
#define CARTRIDGE_TYPE_LOCATION 0x147
#define ROM_SIZE_LOCATION 0x148
#define RAM_SIZE_LOCATION 0x149
#define DESTINATION_CODE_LOCATION 0x14A
#define OLD_LICENSEE_CODE_LOCATION 0x14B 
#define ROM_VERSION_LOCATION 0x14C
#define HEADER_CHECKSUM_LOCATION 0x14D
#define HEADER_CHECKSUM_START 0x134
#define HEADER_CHECKSUM_END 0x14C

namespace mattboy::gameboy::mmu {

  class Cartridge
  {
    public:
      Cartridge(const std::vector<char>& data, const std::string& rom_file);
      ~Cartridge();

      const std::string& GetTitle();

    private:
      // Enum directly reference values in cartridge header
      enum CartridgeType
      {
        ROM_ONLY                = 0x00,
        MBC1                    = 0x01,
        MBC1_RAM                = 0x02,
        MBC1_RAM_BATTERY        = 0x03,
        MBC2                    = 0x05,
        MBC2_BATTERY            = 0x06,
        ROM_RAM                 = 0x08,
        ROM_RAM_BATTERY         = 0x09,
        MMM01                   = 0x0B,
        MMM01_RAM               = 0x0C,
        MMM01_RAM_BATTERY       = 0x0D,
        MBC3_TIMER_BATTERY      = 0x0F,
        MBC3_TIMER_RAM_BATTERY  = 0x10,
        MBC3                    = 0x11,
        MBC3_RAM                = 0x12,
        MBC3_RAM_BATTERY        = 0x13,
        MBC5                    = 0x19,
        MBC5_RAM                = 0x1A,
        MBC5_RAM_BATTERY        = 0x1B,
        MBC5_RUMBLE             = 0x1C,
        MBC5_RUMBLE_RAM         = 0x1D,
        MBC5_RUMBLE_RAM_BATTERY = 0x1E,
        MBC6                    = 0x20,
        MBC7_SENSOR_RUMBLE_RAM_BATTERY = 0x22,
        POCKET_CAMERA           = 0xFC,
        BANDAI_TAMA5            = 0xFD,
        HUC3                    = 0xFE,
        HUC1_RAM_BATTERY        = 0xFF
      } type_;

      static const uint8_t nintendo_logo_[NINTENDO_LOGO_SIZE];
      const std::string rom_file_;
      bool valid_;
      std::string title_;
      int rom_size_;
      int ram_size_;
      bool japanese_dest_;
      uint8_t licensee_code_;
      uint8_t rom_version_;
  };

}

#endif