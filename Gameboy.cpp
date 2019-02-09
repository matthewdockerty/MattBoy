#include "Gameboy.h"

namespace mattboy {

	Gameboy::Gameboy() : running_(false)
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
		// TODO: Reload Cart!
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
			int cycles = cpu_.Cycle(mmu_);
			gpu_.Cycle(cycles, mmu_);
			return cycles;
		}

		return 0;
	}

}