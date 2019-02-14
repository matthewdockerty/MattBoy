#include "InterruptHandler.h"


// TODO: Add to MMU
InterruptHandler::InterruptHandler()
{
	Reset();
}


InterruptHandler::~InterruptHandler()
{
}

void InterruptHandler::Reset()
{
	interrupt_master_enable_ = false;
	interrupt_enable_ = 0x00;
	interrupt_flag_ = 0xe0;
}

void InterruptHandler::SetMasterEnable(bool enabled)
{
	interrupt_master_enable_ = enabled;
}

bool InterruptHandler::IsMasterEnabled()
{
	return interrupt_master_enable_;
}

void InterruptHandler::EnableInterrupt(uint8_t interrupt_bit, bool enabled)
{
	interrupt_enable_ |= enabled << interrupt_bit;
}

bool InterruptHandler::IsInterruptEnabled(uint8_t interrupt_bit)
{
	return (interrupt_enable_ >> interrupt_bit) & 0x1;
}

void InterruptHandler::RequestInterrupt(uint8_t interrupt_bit)
{
	interrupt_flag_ |= 1 << interrupt_bit;
}

bool InterruptHandler::IsInterruptRequested(uint8_t interrupt_bit)
{
	return (interrupt_flag_ >> interrupt_bit) & 0x1;
}

void InterruptHandler::DismissInterrupt(uint8_t interrupt_bit)
{
	interrupt_flag_ &= ~(1 << interrupt_bit);
}

