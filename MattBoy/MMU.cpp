#include "MMU.h"
#include <iostream>

namespace mattboy{

	MMU::MMU(InterruptHandler& interrupt_handler) : cart_(nullptr), memory_{ 0 }, current_rom_bank_(1), current_ram_bank_(0), interrupt_handler_(interrupt_handler), tileViewChanged_(false), backgroundMapViewChanged_(false)
	{

	}

	MMU::~MMU()
	{

	}

	std::shared_ptr<Cartridge> MMU::GetCartridge()
	{
		return cart_;
	}

	void MMU::LoadCartridge(const std::vector<char>& data, const std::wstring& rom_file)
	{
		cart_ = std::make_shared<Cartridge>(data, rom_file);
	}

	void MMU::Reset()
	{
		memset(tileViewPixels_, 0, sizeof(*tileViewPixels_) * TILE_VIEW_WIDTH * TILE_VIEW_HEIGHT);
		memset(backgroundMapViewPixels_, 0, sizeof(*backgroundMapViewPixels_) * BACKGROUND_MAP_VIEW_WIDTH * BACKGROUND_MAP_VIEW_HEIGHT);

		memset(ram_video_, 0, sizeof(*ram_video_) * 0x2000);
		WriteByte(0xFF05, 0x00);
		WriteByte(0xFF06, 0x00);
		WriteByte(0xFF07, 0x00);
		WriteByte(0xFF10, 0x80);
		WriteByte(0xFF11, 0xBF);
		WriteByte(0xFF12, 0xF3);
		WriteByte(0xFF14, 0xBF);
		WriteByte(0xFF16, 0x3F);
		WriteByte(0xFF17, 0x00);
		WriteByte(0xFF19, 0xBF);
		WriteByte(0xFF1A, 0x7F);
		WriteByte(0xFF1B, 0xFF);
		WriteByte(0xFF1C, 0x9F);
		WriteByte(0xFF1E, 0xBF);
		WriteByte(0xFF20, 0xFF);
		WriteByte(0xFF21, 0x00);
		WriteByte(0xFF22, 0x00);
		WriteByte(0xFF23, 0xBF);
		WriteByte(0xFF24, 0x77);
		WriteByte(0xFF25, 0xF3);
		WriteByte(0xFF26, 0xF1);
		WriteByte(0xFF40, 0x91);
		WriteByte(0xFF42, 0x00);
		WriteByte(0xFF43, 0x00);
		WriteByte(0xFF45, 0x00);
		WriteByte(0xFF47, 0xFC);
		WriteByte(0xFF48, 0xFF);
		WriteByte(0xFF49, 0xFF);
		WriteByte(0xFF4A, 0x00);
		WriteByte(0xFF4B, 0x00);
	}

	uint8_t MMU::ReadByte(uint16_t address)
	{
		// ROM bank 0
		if (address < 0x4000)
			return cart_->GetMBC()->GetRomBank0()[address];

		// ROM bank N
		if (address < 0x8000)
			return cart_->GetMBC()->GetROMBankN(current_rom_bank_)[address - 0x4000];

		// Video RAM
		if (address < 0xA000)
			return ram_video_[address - 0x8000];

		// External RAM banks
		if (address < 0xC000)
			return cart_->GetMBC()->GetRAMBankN(current_ram_bank_)[address - 0xA000];

		// Remainder of memory
		if (address <= 0xFFFF)
		{
			if (address == 0xFFFF)
				return interrupt_handler_.GetIE();

			if (address == 0xFF0F)
				return interrupt_handler_.GetIF();

			return memory_[address - 0xC000];
		}

		return 0;
	}

	uint16_t MMU::Read2Bytes(uint16_t address)
	{
		uint8_t low = ReadByte(address);
		uint8_t high = ReadByte(address + 1);

		uint16_t result = (high << 8) | low;

		return result;
	}

	// TODO: Don't let anything be written to readonly memory!
	void MMU::WriteByte(uint16_t address, uint8_t value)
	{
		// ROM banks
		if (address < 0x8000)
		{
			// TODO: READONLY! Handle ROM bank switching
		}
		// Video RAM
		else if (address < 0xA000)
		{

			// Tile data
			if (address >= 0x8000 && address <= 0x97FF)
			{
				// Store a cache of tiles for efficient access and update tile view pixels & background map view pixels
				int tileNum = (address - 0x8000) / 16;
				if (tileNum < TILE_COUNT)
				{
					int tileLine = (address % 16) / 2;

					// value is least significant bits of line
					if (address % 2 == 0)
						tiles_[tileNum].SetLSBitsForLine(tileLine, value);
					else
						tiles_[tileNum].SetMSBitsForLine(tileLine, value);


					// Update tile view
					tileViewChanged_ = true;
					auto tiledata = tiles_[tileNum].pixelsPalette_;

					for (int i = 0; i < 8; i++)
					{
						tileViewPixels_[(8 * tileNum) + (TILE_VIEW_WIDTH * tileLine) + (TILE_VIEW_WIDTH * 7 * (tileNum / TILE_VIEW_ROW_LENGTH)) + i] = GetColorFromPalette(tiledata[tileLine][i]);
					}

					// Update background map view areas which reference the update tile
					/*for (int i = 0x9800; i <= 0x9FFF; i++)
					{
																		  // TODO: 255 here could have off by 1 error?
						int tileIndex = static_cast<uint8_t>(address >= 0x8800 ? tileNum - 255 : tileNum);
						if (ReadByte(i) == (uint8_t)tileIndex)
						{
							int bgTileNum = i - 0x9800;
							if (GetCurrentBGTileData() == 0) // using second tileset with indices -128 to 127
								printf("%d\n", tileIndex);
							auto tiledata = tiles_[tileIndex].pixelsPalette_;

							for (int i = 0; i < 8; i++)
								for (int tileLine = 0; tileLine < 8; tileLine++)
									backgroundMapViewPixels_[(bgTileNum * 8) + (BACKGROUND_MAP_VIEW_WIDTH * tileLine) + (7 * 256 * (bgTileNum / 32)) + i] = GetColorFromPalette(tiledata[tileLine][i]);

							//printf("Updating tile: %d\n", tileIndex);
							// TODO: Update for bgTileNum!!!!
							// !!!!!
							// !!!!!
							// !!!!!
							// !!!!!
							// !!!!!
							// !!!!!
						}
					}*/
				}
			}


			// Background map data
			if (address >= 0x9800 && address <= 0x9FFF)
			{
				backgroundMapViewChanged_ = true;
				int bgTileNum = (int)address - 0x9800;

				/*int tileIndex = static_cast<int>(value);
				if (GetCurrentBGTileData() == 1) // using second tileset with indices -128 to 127
					tileIndex += 128;*/
				auto tiledata = tiles_[GetCurrentBGTileData() == 1 ? value : 0xA].pixelsPalette_;

				for (int i = 0; i < 8; i++)
					for (int tileLine = 0; tileLine < 8; tileLine++)
						backgroundMapViewPixels_[(bgTileNum * 8) + (BACKGROUND_MAP_VIEW_WIDTH * tileLine) + (7 * 256 * (bgTileNum / 32)) + i] = GetColorFromPalette(tiledata[tileLine][i]);

			}

			// TODO: Window background map data...


			ram_video_[address - 0x8000] = value;
		}
		// External RAM banks TODO: Handle saving battery backed RAM to file!
		else if (address < 0xC000)
		{
			cart_->GetMBC()->GetRAMBankN(current_ram_bank_)[address - 0xA000] = value;
		}
		// Remaining memory
		else if (address <= 0xFFFF)
		{
			if (address == 0xFFFF)
			{
				interrupt_handler_.SetIE(value);
			}
			else if (address == 0xFF0F)
			{
				interrupt_handler_.SetIF(value);
			}
			else
			{
				memory_[address - 0xC000] = value;

				// Echo RAM
				if (address >= 0xC000 && address <= 0xDDFF)
					memory_[address - 0xC000 + 0x2000] = value;

				if (address >= 0xE000 && address <= 0xFDFF)
					memory_[address - 0xC000 - 0x2000] = value;
			}
		}

	}

	const Tile& MMU::GetTileById(int tileId)
	{
		return tiles_[tileId];
	}

	const uint8_t * MMU::GetVideoRam()
	{
		return ram_video_;
	}

	const int * MMU::GetTileViewPixels(bool clearChangedFlag)
	{
		tileViewChanged_ &= clearChangedFlag;
		return tileViewPixels_;
	}

	bool MMU::HasTileViewChanged()
	{
		return tileViewChanged_;
	}

	const int * MMU::GetBackgroundMapViewPixels(bool clearChangedFlag)
	{
		backgroundMapViewChanged_ &= clearChangedFlag;
		return backgroundMapViewPixels_;
	}

	bool MMU::HasBackgroundMapViewChanged()
	{
		return backgroundMapViewChanged_;
	}

	uint32_t MMU::GetColorFromPalette(uint8_t paletteValue)
	{
		uint8_t paletteData = ReadByte(0xFF47);
		uint8_t shade = paletteData >> (2 * paletteValue) & 0b11;

		switch (shade) {
		case 0:
			return 0x9bbc0f;
		case 1:
			return 0x8bac0f;
		case 2:
			return 0x306230;
		case 3:
			return 0x0f380f;
		}
	}

	
	int MMU::GetCurrentBGTileData()
	{
		return (ReadByte(0xFF40) >> (7-4)) & 0b1;
	}

}