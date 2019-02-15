#include "Gameboy.h"

#include <iostream>

namespace mattboy {

	Gameboy::Gameboy() : mmu_(interrupt_handler_), running_(false)
	{

	}

	Gameboy::~Gameboy()
	{
	}

	void Gameboy::Reset()
	{
		mmu_.Reset();
		cpu_.Reset();
		gpu_.Reset();
		interrupt_handler_.Reset();
		// TODO: Reload Cart from file!
	}

	void Gameboy::LoadCartridge(const std::vector<char>& data, const std::wstring& rom_file)
	{
		mmu_.LoadCartridge(data, rom_file);
		if (mmu_.GetCartridge()->IsValid())
			Reset();
	}

	std::shared_ptr<Cartridge> Gameboy::GetCartridge()
	{
		return mmu_.GetCartridge();
	}

	void Gameboy::SetRunning(bool value)
	{
		running_ = value;
	}

	int Gameboy::Cycle()
	{
		if (running_ && mmu_.GetCartridge() != nullptr && mmu_.GetCartridge()->IsValid())
		{
			int cycles = cpu_.Cycle(mmu_, interrupt_handler_);
			gpu_.Cycle(cycles * 2, mmu_, interrupt_handler_);

#ifdef PRINT_STATE
			printf("IME: %02x   IE: %02x   IF: %02x\n\n", interrupt_handler_.IsMasterEnabled(), interrupt_handler_.GetIE(), interrupt_handler_.GetIF());
#endif

			if (interrupt_handler_.IsMasterEnabled())
			{
				if (interrupt_handler_.IsInterruptEnabled(InterruptHandler::VBLANK) && interrupt_handler_.IsInterruptRequested(InterruptHandler::VBLANK))
				{
					printf("VBLANK INTERRUPT!!\n");
					interrupt_handler_.SetMasterEnable(false);
					interrupt_handler_.DismissInterrupt(InterruptHandler::VBLANK);
					cpu_.HandleInterrupt(mmu_, InterruptHandler::VBLANK_HANDLER);
				}
				else if (interrupt_handler_.IsInterruptEnabled(InterruptHandler::LCD_STAT) && interrupt_handler_.IsInterruptRequested(InterruptHandler::LCD_STAT))
				{
					interrupt_handler_.SetMasterEnable(false);
					interrupt_handler_.DismissInterrupt(InterruptHandler::LCD_STAT);
					cpu_.HandleInterrupt(mmu_, InterruptHandler::LCD_STAT_HANDLER);
				}
				else if (interrupt_handler_.IsInterruptEnabled(InterruptHandler::TIMER) && interrupt_handler_.IsInterruptRequested(InterruptHandler::TIMER))
				{
					interrupt_handler_.SetMasterEnable(false);
					interrupt_handler_.DismissInterrupt(InterruptHandler::TIMER);
					cpu_.HandleInterrupt(mmu_, InterruptHandler::TIMER_HANDLER);
				}
				else if (interrupt_handler_.IsInterruptEnabled(InterruptHandler::SERIAL) && interrupt_handler_.IsInterruptRequested(InterruptHandler::SERIAL))
				{
					interrupt_handler_.SetMasterEnable(false);
					interrupt_handler_.DismissInterrupt(InterruptHandler::SERIAL);
					cpu_.HandleInterrupt(mmu_, InterruptHandler::SERIAL_HANDLER);
				}
				else if (interrupt_handler_.IsInterruptEnabled(InterruptHandler::JOYPAD) && interrupt_handler_.IsInterruptRequested(InterruptHandler::JOYPAD))
				{
					interrupt_handler_.SetMasterEnable(false);
					interrupt_handler_.DismissInterrupt(InterruptHandler::JOYPAD);
					cpu_.HandleInterrupt(mmu_, InterruptHandler::JOYPAD_HANDLER);
				}
			}

			return cycles;
		}

		return 0;
	}

}