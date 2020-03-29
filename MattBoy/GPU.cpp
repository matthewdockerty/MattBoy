#include "GPU.h"

#include <iostream>

namespace mattboy {

	GPU::GPU() : mode_(HBLANK), current_cycles_(0), line_(0)
	{
		Reset();
	}

	GPU::~GPU()
	{

	}

	// TODO: Write values to memory & change MMU to only be able to access memory at the correct times.
	void GPU::Cycle(int cycles, MMU& mmu, InterruptHandler& interrupt_handler)
	{
		current_cycles_ += cycles;

		switch (mode_)
		{
		case HBLANK:
			if (current_cycles_ >= 204)
			{
				line_++;

				if (line_ > 143)
				{
					mode_ = VBLANK;
					// TODO: Should redraw screen
					//printf("VBLANK!\n");
					interrupt_handler.RequestInterrupt(InterruptHandler::VBLANK);
				}
				else
				{
					mode_ = OAM;
				}
			}
			break;

		case VBLANK:
			if (current_cycles_ >= 456)
			{
				current_cycles_ -= 456;
				line_++;

				if (line_ > 153) // Overdraw 10 lines during vblank
				{
					mode_ = OAM;
					line_ = 0;
				}
			}
			break;

		case OAM:
			if (current_cycles_ >= 80)
			{
				mode_ = VRAM;
			}
			break;

		case VRAM:
			if (current_cycles_ >= 172)
			{
				mode_ = HBLANK;
				// TODO: Render line

				const uint8_t *vram = mmu.GetVideoRam();
				
				if (line_ == 140)
				DrawTilesetToScreen(mmu);
			}
			break;
		}
		mmu.WriteByte(0xFF44, line_);
	}

	void GPU::ExtractTileDataFromVRAM(uint8_t tile_id, uint8_t tileData[][8], const uint8_t *vram)
	{
		for (int i = 0; i < 8; i++) {
			uint8_t line1 = vram[(tile_id * 16) + (i * 2)];
			uint8_t line2 = vram[(tile_id * 16) + (i * 2) + 1];

			for (int j = 0; j < 8; j++) {
				tileData[i][8 - j - 1] = static_cast<uint8_t>(((line1 >> (j - 1)) & 0b10) | (line2 >> (j)) & 0b1);
			}
		}
	}

	uint32_t GPU::GetColorFromPalette(MMU& mmu, uint8_t paletteValue)
	{
		uint8_t paletteData = mmu.ReadByte(0xFF47);
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

	void GPU::DrawTilesetToScreen(MMU& mmu)
	{

		for (int offset = 0; offset < MMU::TILE_COUNT; offset++) {
			//ExtractTileDataFromVRAM(offset, tiledata, mmu.GetVideoRam());
			auto tiledata = mmu.GetTileById(offset).pixelsPalette_;
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					screen_[(8 * offset) + (SCREEN_WIDTH * j) + (SCREEN_WIDTH * 8 * (offset / 20)) + i] = GetColorFromPalette(mmu, tiledata[j][i]);
				}
			}
		}
	}

	void GPU::Reset()
	{
		std::cout << "TODO: GPU Reset!" << std::endl;

		for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
			screen_[i] = 0x9bbc0f;
	}

	int * GPU::GetScreen()
	{
		return screen_;
	}

}