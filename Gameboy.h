#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include "MMU.h"
#include "CPU.h"
#include "GPU.h"
#include "InterruptHandler.h"

namespace mattboy {

	class Gameboy
	{
	public:
		Gameboy();
		~Gameboy();

		void Reset();
		void LoadCartridge(const std::vector<char>& data, const std::wstring& rom_file);
		std::shared_ptr<Cartridge> GetCartridge();

		int Cycle();
		void SetRunning(bool value);

		int *GetScreen();

	private:
		InterruptHandler interrupt_handler_;
		MMU mmu_;
		CPU cpu_;
		GPU gpu_;

		bool running_;
	};

}

#endif