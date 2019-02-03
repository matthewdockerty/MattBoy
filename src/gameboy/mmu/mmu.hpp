#ifndef MMU_H_
#define MMU_H_

#include "cartridge.hpp"
#include <cstdint>
#include <memory>

namespace mattboy::gameboy::mmu {

  class MMU
  {
    public:
      MMU();
      ~MMU();

      std::shared_ptr<Cartridge> GetCartridge();
      void LoadCartridge(const std::vector<char>& data);
      void Reset();

    private:
      std::shared_ptr<Cartridge> cart_;

      uint8_t memory_[0xFFFFFF];

      // rom_0_[0x4000];
      // rom_n_[0x4000];
      // ram_video_[0x2000];
      // ram_external_[0x2000];
      // ram_work_0_[0x1000];
      // ram_work_n_[0x1000];
      // ram_echo_[0x1E00];
      // attrib_table_[0xA0];
      // unusable_[0x60];
      // io_registers_[0x80];
      // high_ram_[0x7F];
      // interrupt_enable_;
  };

}

#endif