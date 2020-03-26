#ifndef GPU_H_
#define GPU_H_

#include "MMU.h"
#include "GPUMode.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144


namespace mattboy {

	class GPU
	{
	public:
		GPU();
		~GPU();

		void Cycle(int cycles, MMU& mmu, InterruptHandler& interrupt_handler);
		void Reset();

		int *GetScreen();

	private:
		GPUMode mode_;
		int current_cycles_;

		int screen_[SCREEN_WIDTH * SCREEN_HEIGHT];

		// LY 0xFF44
		uint8_t line_;

		// LCD Control Register (LCDC 0xFF40)
		uint8_t lcdc_;
		static const int LCDC_LCD_DISPLAY_ENABLE = 7;
		static const int LCDC_WINDOW_TILEMAP_DISPLAY_SELECT = 6;
		static const int LCDC_WINDOW_DISPLAY_ENABLE = 5;
		static const int LCDC_BG_WINDOW_TILE_DATA_SELECT = 4;
		static const int LCDC_BG_TILE_MAP_DISPLAY_SELECT = 3;
		static const int LCDC_SPRITE_SIZE = 2;
		static const int LCDC_SPRITE_DISPLAY_ENABLE = 1;
		static const int LCDC_BG_DISPLAY = 0;

		// LCD Status Register (STAT 0xFF41)
		uint8_t stat_;
		static const int STAT_LYC_LY_COINCIDENCE_INTERRUPT_ENABLE = 6;
		static const int STAT_OAM_INTERRUPT_ENABLE = 5;
		static const int STAT_VBLANK_INTERRUPT_ENABLE = 4;
		static const int STAT_HBLANK_INTERRUPT_ENABLE = 3;
		static const int STAT_LYC_LY_COINCIDENCE = 2;
		static const int STAT_MODE_FLAG = 0;

		void ExtractTileDataFromVRAM(uint8_t tile_id, uint8_t tileData[][8], const uint8_t *vram);
		uint32_t GetColorFromPalette(MMU& mmu, uint8_t paletteValue);
		void DrawTilesetToScreen(MMU& mmu);

	};

}

#endif