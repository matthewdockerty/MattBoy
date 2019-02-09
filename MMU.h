#ifndef MMU_H_
#define MMU_H_

#include "Cartridge.h"
#include <cstdint>
#include <memory>

namespace mattboy {

	class MMU
	{
	public:
		MMU();
		~MMU();

		std::shared_ptr<Cartridge> GetCartridge();
		void LoadCartridge(const std::vector<char>& data, const std::wstring& rom_file);
		void Reset();

		uint8_t ReadByte(uint16_t address);
		uint16_t Read2Bytes(uint16_t address);
		void WriteByte(uint16_t address, uint8_t value);

	private:
		std::shared_ptr<Cartridge> cart_;

		// ROM banks & external RAM are handled by MBC
		uint8_t ram_video_[0x2000];
		uint8_t memory_[0x4000];

		int current_rom_bank_;
		int current_ram_bank_;

		// rom_0_[0x4000];
		// rom_n_[0x4000];
		// ram_video_[0x2000];
		// ram_external_[0x2000];
		// ram_work_0_[0x1000];
		// ram_work_n_[0x1000]; (only up to bank 1 in non CGB)
		// ram_echo_[0x1E00];
		// attrib_table_[0xA0];
		// unusable_[0x60];
		// io_registers_[0x80];
		// high_ram_[0x7F];
		// interrupt_enable_;
	};

}

#endif