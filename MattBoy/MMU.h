#ifndef MMU_H_
#define MMU_H_

#include "Cartridge.h"
#include <cstdint>
#include <memory>

#include "InterruptHandler.h"
#include "Tile.h"

namespace mattboy {
	class MMU
	{
	public:
		MMU(InterruptHandler& interrupt_handler);
		~MMU();

		std::shared_ptr<Cartridge> GetCartridge();
		void LoadCartridge(const std::vector<char>& data, const std::wstring& rom_file);
		void Reset();

		uint8_t ReadByte(uint16_t address);
		uint16_t Read2Bytes(uint16_t address);
		void WriteByte(uint16_t address, uint8_t value);

		const uint8_t * GetVideoRam();
		const Tile& GetTileById(int tileId);
		const int * GetTileViewPixels(bool clearChangedFlag);
		bool HasTileViewChanged();
		const int * GetBackgroundMapViewPixels(bool clearChangedFlag);
		bool HasBackgroundMapViewChanged();

		static const int TILE_COUNT = 255 + 127;
		static const int TILE_VIEW_ROW_LENGTH = 24;
		static const int TILE_VIEW_WIDTH = TILE_VIEW_ROW_LENGTH * 8, TILE_VIEW_HEIGHT = 128;

		static const int BACKGROUND_MAP_VIEW_WIDTH = 32 * 8;
		static const int BACKGROUND_MAP_VIEW_HEIGHT = 32 * 8 * 2;

		uint32_t GetColorFromPalette(uint8_t paletteValue);

	private:
		int tileViewPixels_[TILE_VIEW_WIDTH * TILE_VIEW_HEIGHT];
		bool tileViewChanged_;

		int backgroundMapViewPixels_[BACKGROUND_MAP_VIEW_WIDTH * BACKGROUND_MAP_VIEW_HEIGHT];
		bool backgroundMapViewChanged_;

		std::shared_ptr<Cartridge> cart_;

		// ROM banks & external RAM are handled by MBC
		uint8_t ram_video_[0x2000];
		uint8_t memory_[0x4000];

		int current_rom_bank_;
		int current_ram_bank_;

		InterruptHandler& interrupt_handler_;

		Tile tiles_[TILE_COUNT];

		int GetCurrentBGTileData();

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