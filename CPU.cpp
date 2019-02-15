#include "CPU.h"

#include <iostream>

namespace mattboy {

	CPU::CPU()
	{
		
	}

	CPU::~CPU()
	{

	}

	void CPU::Reset()
	{
		SetRegister(REG_A, 0x01);
		SetRegister(REG_F, 0xB0);
		SetRegisterPair(REG_B, REG_C, 0x0013);
		SetRegisterPair(REG_D, REG_E, 0x00D8);
		SetRegisterPair(REG_H, REG_L, 0x014D);
		sp_ = 0xFFFE;
		pc_ = 0x100;
		interrupt_wait_cycles_ = 0;
		pending_disable_interrupts_ = false;
	}

	void CPU::HandleInterrupt(MMU& mmu, uint16_t handler_address)
	{
		printf("INTERRUPT HANDLER!\n");
		// TODO: wait 2 cycles...
		interrupt_wait_cycles_ += 8; // Wait 2 machine cycles when the interrupt service routine is executed.
		
		PushStack(mmu, pc_);
		interrupt_wait_cycles_ += 8;
		
		pc_ = handler_address;
		interrupt_wait_cycles_ += 4;
	}

	int CPU::Cycle(MMU& mmu, InterruptHandler& interrupt_handler)
	{
		int cycles = interrupt_wait_cycles_; // CPU cycle count used for instruction (measured in CLOCK CYCLES, with a 4.19MHz clockspeed)
		interrupt_wait_cycles_ = 0;

		if (pending_disable_interrupts_)
		{
			pending_disable_interrupts_ = false;
			interrupt_handler.SetMasterEnable(false);
		}

		uint8_t instruction = mmu.ReadByte(pc_);
		printf("instruction: %02x   pc: %04x\n", instruction, pc_);
		pc_++;

		switch (instruction)
		{
		case 0x00: // NOP
			cycles += 4;
			break;

		case 0x01: // LD BC,nn
			cycles += 12;
			SetRegisterPair(REG_B, REG_C, mmu.Read2Bytes(pc_));
			pc_ += 2;
			break;

		case 0x03: // INC BC
			cycles += 8;
			SetRegisterPair(REG_B, REG_C, GetRegisterPair(REG_B, REG_C) + 1);
			break;

		case 0x05: // DEC B
			cycles += 4;
			REG_B--;
			SetFlag(FLAG_ZERO, REG_B == 0);
			SetFlag(FLAG_ADD_SUB, true);
			SetFlag(FLAG_HALF_CARRY, (REG_B & 0x0F) == 0x0F);
			break;

		case 0x06: // LD B n
			cycles += 8;
			SetRegister(REG_B, mmu.ReadByte(pc_++));
			break;

		case 0x0B: // DEC BC
		{
			cycles += 8;
			uint16_t bc = GetRegisterPair(REG_B, REG_C);
			SetRegisterPair(REG_B, REG_C, bc - 1);
		}
		break;

		case 0x0C: // INC C
			cycles += 4;
			REG_C++;
			SetFlag(FLAG_ZERO, REG_C == 0);
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_HALF_CARRY, (REG_C & 0x0F) == 0x00);
			break;

		case 0x0D: // DEC C
			cycles += 4;
			REG_C--;
			SetFlag(FLAG_ZERO, REG_C == 0);
			SetFlag(FLAG_ADD_SUB, true);
			SetFlag(FLAG_HALF_CARRY, (REG_C & 0x0F) == 0x0F);
			break;

		case 0x0e: // LD C,n
			cycles += 8;
			SetRegister(REG_C, mmu.ReadByte(pc_++));
			break;

		case 0x12: // LD (DE),A
			cycles += 8;
			mmu.WriteByte(GetRegisterPair(REG_D, REG_E), REG_A);
			break;

		case 0x13: // INC DE
			cycles += 8;
			SetRegisterPair(REG_D, REG_E, GetRegisterPair(REG_D, REG_E) + 1);
			break;

		case 0x16: // LD D,n
			cycles += 8;
			REG_D = mmu.ReadByte(pc_++);
			break;

		case 0x19: // ADD HL,DE
		{
			cycles += 8;
			uint16_t hl = GetRegisterPair(REG_H, REG_L);
			uint16_t de = GetRegisterPair(REG_D, REG_E);
			int result = hl + de;
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_CARRY, (result & 0x10000) != 0);
			SetFlag(FLAG_HALF_CARRY, ((hl ^ de ^ (result & 0xFFFF)) & 0x1000) != 0);
			SetRegisterPair(REG_H, REG_L, static_cast<uint16_t>(result));
		}
		break;

		case 0x20: // JR NZ,n
			if (!CheckFlag(FLAG_ZERO))
			{
				cycles += 12;
				pc_ += (int8_t)mmu.ReadByte(pc_) + 1;
			}
			else
			{
				cycles += 8;
				pc_++;
			}
			break;

		case 0x21: // LD HL nn
			cycles += 12;
			SetRegisterPair(REG_H, REG_L, mmu.Read2Bytes(pc_));
			pc_ += 2;
			break;

		case 0x23: // INC HL
			cycles += 8;
			SetRegisterPair(REG_H, REG_L, GetRegisterPair(REG_H, REG_L) + 1);
			break;

		case 0x2A: // LD A,(HL+)
		{
			cycles += 8;
			uint16_t hl = GetRegisterPair(REG_H, REG_L);
			SetRegister(REG_A, mmu.ReadByte(hl));
			SetRegisterPair(REG_H, REG_L, hl + 1);
		}
		break;

		case 0x2F: // CPL
			cycles += 4;
			REG_A = ~REG_A;
			SetFlag(FLAG_ADD_SUB, true);
			SetFlag(FLAG_HALF_CARRY, true);
			break;

		case 0x31: // LD SP,nn
			cycles += 12;
			sp_ = mmu.Read2Bytes(pc_);
			pc_ += 2;
			break;

		case 0x32: // LD (HL-),A
		{
			cycles += 8;
			uint16_t hl = GetRegisterPair(REG_H, REG_L);
			mmu.WriteByte(hl, REG_A);
			SetRegisterPair(REG_H, REG_L, hl - 1);
		}
		break;

		case 0x33: // INC SP
			cycles += 8;
			sp_++;
			break;

		case 0x36: // LD (HL), n
			cycles += 12;
			mmu.WriteByte(GetRegisterPair(REG_H, REG_L), mmu.ReadByte(pc_++));
			break;

		case 0x3E: // LD A,#
			cycles += 8;
			SetRegister(REG_A, mmu.ReadByte(pc_++));
			break;

		case 0x47: // LD B,A
			cycles += 4;
			REG_B = REG_A;
			break;

		case 0x4F: // LD C,A
			cycles += 4;
			REG_C = REG_A;
			break;

		case 0x56: // LD D,(HL)
			cycles += 8;
			REG_D = mmu.ReadByte(GetRegisterPair(REG_H, REG_L));
			break;

		case 0x5E: // LD E,(HL)
			cycles += 8;
			REG_E = mmu.ReadByte(GetRegisterPair(REG_H, REG_L));
			break;

		case 0x5F: // LD E,A
			cycles += 4;
			REG_E = REG_A;
			break;

		case 0x78: // LD A,B
			cycles += 4;
			SetRegister(REG_A, REG_B);
			break;

		case 0x79: // LD A,C
			cycles += 4;
			REG_A = REG_C;
			break;

		case 0x87: // ADD A,A
			cycles += 4;
			REG_A += REG_A;
			SetFlag(FLAG_ZERO, REG_A == 0);
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_HALF_CARRY, (REG_A & 0x0F) + (REG_A & 0x0F) > 0x0F);
			SetFlag(FLAG_CARRY, (((REG_A & 0x0FF) + (REG_A & 0x0FF)) & 0x100) != 0);
			break;

		case 0xA1: // AND C
			cycles += 4;
			REG_A = REG_A & REG_C;
			SetFlag(FLAG_ZERO, REG_A == 0);
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_HALF_CARRY, true);
			SetFlag(FLAG_CARRY, false);
			break;

		case 0xA7: // AND A
			cycles += 4;
			REG_A &= REG_A;
			SetFlag(FLAG_ZERO, REG_A == 0);
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_HALF_CARRY, true);
			SetFlag(FLAG_CARRY, false);
			break;

		case 0xA9: // XOR C
			cycles += 4;
			REG_A = REG_A ^ REG_C;
			SetFlag(FLAG_ZERO, REG_A == 0);
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_HALF_CARRY, false);
			SetFlag(FLAG_CARRY, false);
			break;

		case 0xAF: // XOR A
			cycles += 4;
			REG_A ^= REG_A;
			SetFlag(FLAG_ZERO, REG_A == 0);
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_HALF_CARRY, false);
			SetFlag(FLAG_CARRY, false);
			break;

		case 0xB0: // OR B
			cycles += 4;
			SetRegister(REG_A, REG_A | REG_B);
			SetFlag(FLAG_ZERO, REG_A == 0);
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_HALF_CARRY, false);
			SetFlag(FLAG_CARRY, false);
			break;

		case 0xB1: // OR C
			cycles += 4;
			SetRegister(REG_A, REG_A | REG_C);
			SetFlag(FLAG_ZERO, REG_A == 0);
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_HALF_CARRY, false);
			SetFlag(FLAG_CARRY, false);
			break;

		case 0xC0: // RET NZ
			if (CheckFlag(FLAG_ZERO))
			{
				cycles += 8;
			}
			else
			{
				cycles += 20;
				pc_ = mmu.Read2Bytes(sp_);
				sp_ += 2;
			}
			break;

		case 0xC3: // Jump to a memory address
			cycles += 16;
			pc_ = mmu.Read2Bytes(pc_);
			break;

		case 0xC9: // RET 
		{
			cycles += 16;
			pc_ = mmu.Read2Bytes(sp_);
			sp_ += 2;
		}
		break;

		case 0xCB: // PREFIX CB
			cycles += 4;
			instruction = mmu.ReadByte(pc_++);

			switch (instruction)
			{
			case 0x37: // SWAP A
				cycles += 8;
				REG_A = (REG_A << 4) | (REG_A >> 4);
				SetFlag(FLAG_ZERO, REG_A == 0);
				SetFlag(FLAG_ADD_SUB, false);
				SetFlag(FLAG_HALF_CARRY, false);
				SetFlag(FLAG_CARRY, false);
				break;

			default:
				printf("unimplemented opcode: 0xCB 0x%02x   pc: %x\n", instruction, pc_ - 2);
				break;
			}

			break;

		case 0xCA: // JP Z,nn
			if (CheckFlag(FLAG_ZERO))
			{
				cycles += 16;
				pc_ = mmu.Read2Bytes(pc_);
			}
			else
			{
				cycles += 12;
				pc_ += 2;
			}
			break;

		case 0xCD: // CALL nn
		{
			cycles += 24;

			uint16_t address = mmu.Read2Bytes(pc_);
			pc_ += 2;
			PushStack(mmu, pc_);
			pc_ = address;
		}
		break;

		case 0xD5: // PUSH DE
			cycles += 16;
			PushStack(mmu, GetRegisterPair(REG_D, REG_E));
			break;

		case 0xE0: // LD ($FF00+n),A
			cycles += 12;
			printf("!!!%0x4\n", 0xFF00 + mmu.ReadByte(pc_++));
			mmu.WriteByte((uint16_t)0xFF00 + mmu.ReadByte(pc_++), REG_A);
			break;

		case 0xE1: // POP HL
			cycles += 12;
			SetRegisterPair(REG_H, REG_L, mmu.Read2Bytes(sp_));
			sp_ += 2;
			break;

		case 0xE2: // LD (C),A
			cycles += 8;
			mmu.WriteByte(0xFF00 + REG_C, REG_A);
			break;

		case 0xE6: // AND #
			cycles += 8;
			REG_A = REG_A & mmu.ReadByte(pc_++);
			SetFlag(FLAG_ZERO, REG_A == 0);
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_HALF_CARRY, true);
			SetFlag(FLAG_CARRY, false);
			break;

		case 0xE9: // JP (HL)
			cycles += 4;
			pc_ = GetRegisterPair(REG_H, REG_L);
			break;

		case 0xEA: // LD (nn),A
			cycles += 16;
			mmu.WriteByte(mmu.Read2Bytes(pc_), REG_A);
			pc_ += 2;
			break;

		case 0xEF: // RST 28H
			cycles += 16;
			PushStack(mmu, pc_);
			pc_ = 0x28;
			break;

		case 0xF0: // LD A,($FF00+n)
			cycles += 12;
			SetRegister(REG_A, mmu.ReadByte(0xFF00 + mmu.ReadByte(pc_++)));
			break;

		case 0xF3: // DI
			cycles += 4;
			pending_disable_interrupts_ = true;
			break;

		case 0xFB: // EI
			cycles += 4;
			interrupt_handler.SetMasterEnable(true);
			break;

		case 0xFE: // CP n
		{
			cycles += 8;
			uint8_t n = mmu.ReadByte(pc_++);
			uint8_t result = REG_A - n;
			SetFlag(FLAG_ZERO, result == 0);
			SetFlag(FLAG_ADD_SUB, true);
			SetFlag(FLAG_HALF_CARRY, (REG_A & 0xF) - (n & 0xF) < 0);
			SetFlag(FLAG_CARRY, REG_A < n);
		}
		break;

		default:
			printf("unimplemented opcode: 0x%02x   pc: %x\n", instruction, pc_ - 1);
			break;
		}


		printf("A: %02x   F: %02x\n", REG_A, REG_F);
		printf("B: %02x   C: %02x\n", REG_B, REG_C);
		printf("D: %02x   E: %02x\n", REG_D, REG_E);
		printf("H: %02x   L: %02x\n", REG_H, REG_L);
		printf("LY: %02x\n", mmu.ReadByte(0xFF44));

		return cycles;
	}

}