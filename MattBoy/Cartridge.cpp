#include "Cartridge.h"

#include <iostream>
#include "CartridgeType.h"
#include "MBCNone.h"

namespace mattboy {

	const uint8_t Cartridge::nintendo_logo_[] = {
		0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
		0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
		0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
	};

	Cartridge::Cartridge(const std::vector<char>& data, const std::wstring& rom_file) : rom_file_(rom_file), valid_(true)
	{
		// Verify the content of the Nintendo logo bitmap
		for (int i = 0; i < NINTENDO_LOGO_SIZE; i++)
		{
			if (static_cast<uint8_t>(data[NINTENDO_LOGO_LOCATION + i]) != nintendo_logo_[i])
			{
				valid_ = false;
				break;
			}
		}

		/** Title of the ROM - initially 16 characters, reduced to 15, then to 11.
		* The last 5 bytes can contain the manufacturer code (013F-0142) & CGB flag (0143) in newer cartridges.
		* Here we include all possible title bytes in the title field & ignore manufacturer code */
		title_ = std::string(&data[TITLE_LOCATION], &data[TITLE_LOCATION] + TITLE_SIZE).append("\0");

		// Check if the game is a backwards compatible game for the CGB
		valid_ = static_cast<uint8_t>(data[CGB_FLAG_LOCATION]) != CGB_FLAG_VALUE;

		// Cartridge type - specifies which MBC the cartridge uses and any additional hardware.
		CartridgeType type = static_cast<CartridgeType>(static_cast<uint8_t>(data[CARTRIDGE_TYPE_LOCATION]));

		// Size of the ROM in the cartridge (in bytes)
		int rom_size = (1024 * 32) << static_cast<int>(data[ROM_SIZE_LOCATION]);

		int ram_size = 0;
		// Size of the RAM in the cartridge (in bytes)
		switch (data[RAM_SIZE_LOCATION])
		{
		case 0x00:
			// no RAM
			break;
		case 0x01:
			ram_size = 2 * 1024;
			break;
		case 0x02:
			ram_size = 8 * 1024;
			break;
		case 0x03:
			ram_size = 32 * 1024;
			break;
		case 0x04:
			ram_size = 128 * 1024;
			break;
		case 0x05:
			ram_size = 64 * 1024;
			break;
		}

		// Whether the game was supposed to be sold in Japan or elsewhere
		japanese_dest_ = data[DESTINATION_CODE_LOCATION] == 0x00;

		if (data[OLD_LICENSEE_CODE_LOCATION] == 0x33)
		{
			// TODO: Use new licensee code (GBC?)
			licensee_code_ = 0;
		}
		else
		{
			licensee_code_ = static_cast<uint8_t>(data[OLD_LICENSEE_CODE_LOCATION]);
		}

		// ROM version number (usually 0x00)
		rom_version_ = static_cast<uint8_t>(data[ROM_VERSION_LOCATION]);

		// 8 bit header checksum
		uint8_t checksum = 0;
		for (int i = HEADER_CHECKSUM_START; i <= HEADER_CHECKSUM_END; i++)
		{
			checksum = checksum - static_cast<uint8_t>(data[i]) - 1;
		}
		if (checksum != static_cast<uint8_t>(data[HEADER_CHECKSUM_LOCATION]))
			valid_ = false;

		// Create memory bank controller
		switch (type)
		{
		case ROM_ONLY:
		case MBC1:
		case ROM_RAM:
		case ROM_RAM_BATTERY:
			mem_bank_controller_ = std::make_shared<MBCNone>(type, rom_size, ram_size, type, data);
			break;
		default:
			std::cerr << "ROM type: " << type << " unimplemented" << std::endl;
			valid_ = false;
			break;
		}
	}

	Cartridge::~Cartridge()
	{

	}

	const std::string& Cartridge::GetTitle()
	{
		return title_;
	}

	std::shared_ptr<MBC> Cartridge::GetMBC()
	{
		return mem_bank_controller_;
	}

	bool Cartridge::IsValid()
	{
		return valid_;
	}
}
