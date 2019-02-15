#ifndef INTERRUPT_HANDLER_H_
#define INTERRUPT_HANDLER_H_

#include <cstdint>

class InterruptHandler
{
public:
	InterruptHandler();
	~InterruptHandler();

	void Reset();
	void SetMasterEnable(bool enabled);
	bool IsMasterEnabled();
	void EnableInterrupt(uint8_t interrupt_bit, bool enabled);
	bool IsInterruptEnabled(uint8_t interrupt_bit);
	void RequestInterrupt(uint8_t interrupt_bit);
	bool IsInterruptRequested(uint8_t interrupt_bit);
	void DismissInterrupt(uint8_t interrupt_bit);

	uint8_t GetIE();
	uint8_t GetIF();
	void SetIE(uint8_t value);
	void SetIF(uint8_t value);

	static const uint8_t VBLANK = 0;
	static const uint8_t LCD_STAT = 1;
	static const uint8_t TIMER = 2;
	static const uint8_t SERIAL = 3;
	static const uint8_t JOYPAD = 4;

	static const uint16_t VBLANK_HANDLER = 0x40;
	static const uint16_t LCD_STAT_HANDLER = 0x48;
	static const uint16_t TIMER_HANDLER = 0x50;
	static const uint16_t SERIAL_HANDLER = 0x58;
	static const uint16_t JOYPAD_HANDLER = 0x60;

private:
	bool interrupt_master_enable_; // Write only - not accessible at a memory address
	uint8_t interrupt_enable_; // R/W - memory location 0xFFFF
	uint8_t interrupt_flag_; // R/W - memory location 0xFF0F
};

#endif
