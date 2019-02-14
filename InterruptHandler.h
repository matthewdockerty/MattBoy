#ifndef INTERRUPT_HANDLER_H_
#define INTERRUPT_HANDLER_H_

#define VBLANK_BIT 0
#define LCD_STAT_BIT 1
#define TIMER_BIT 2
#define SERIAL_BIT 3
#define JOYPAD_BIT 4

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

private:
	bool interrupt_master_enable_; // Write only - not accessible at a memory address
	uint8_t interrupt_enable_; // R/W - memory location 0xFFFF
	uint8_t interrupt_flag_; // R/W - memory location 0xFF0F
};

#endif
