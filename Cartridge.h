#ifndef CARTRIDGE_H_
#define CARTRIDGE_H_

#include <vector>
#include <cstdint>
#include <string>
#include <memory>
#include "MBC.h"

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

namespace mattboy {

	class Cartridge
	{
	public:
		Cartridge(const std::vector<char>& data, const std::wstring& rom_file);
		~Cartridge();

		const std::string& GetTitle();
		std::shared_ptr<MBC> GetMBC();

		bool IsValid();

	private:

		static const uint8_t nintendo_logo_[NINTENDO_LOGO_SIZE];
		const std::wstring rom_file_;
		bool valid_;
		std::string title_;
		bool japanese_dest_;
		uint8_t licensee_code_;
		uint8_t rom_version_;

		std::shared_ptr<MBC> mem_bank_controller_;

	};

}

#endif