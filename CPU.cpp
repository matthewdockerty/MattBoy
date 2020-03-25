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
		pending_enable_interrupts_ = false;
	}

	void CPU::HandleInterrupt(MMU& mmu, uint16_t handler_address)
	{
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

		if (pending_enable_interrupts_)
		{
			pending_enable_interrupts_ = false;
			interrupt_handler.SetMasterEnable(true);
		}

		uint8_t instruction = mmu.ReadByte(pc_);

		static bool debug = false;
		//if (pc_ == 0x2c2)
			//debug = true;

		if (pc_ == 0x282a) {
			uint8_t* vmem = mmu.GetVideoRam();
			for (int i = 0; i < 0x2000; i++) {
				printf("%x ", vmem[i]);
			}
			system("pause");
		}

#ifdef PRINT_STATE
		printf("A: %02x   F: %02x\n", REG_A, REG_F);
		printf("B: %02x   C: %02x\n", REG_B, REG_C);
		printf("D: %02x   E: %02x\n", REG_D, REG_E);
		printf("H: %02x   L: %02x\n", REG_H, REG_L);
		printf("LY: %02x\n", mmu.ReadByte(0xFF44));

		printf("instruction: %02x   pc: %04x\n", instruction, pc_);
		if (debug)
			system("pause");
#endif

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

		case 0x02: // LD (BC),A
			mmu.WriteByte(GetRegisterPair(REG_B, REG_C), REG_A);
			cycles += 8;
			break;

		case 0x03: // INC BC
			cycles += 8;
			SetRegisterPair(REG_B, REG_C, GetRegisterPair(REG_B, REG_C) + 1);
			break;

		case 0x04: // INC B
			cycles += 4;
			REG_B++;
			SetFlag(FLAG_ZERO, REG_B == 0);
			SetFlag(FLAG_ADD_SUB, true);
			SetFlag(FLAG_HALF_CARRY, (REG_B & 0x0F) == 0x0F);
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

		case 0x09: // ADD HL,BC
		{
			cycles += 8;
			uint16_t hl = GetRegisterPair(REG_H, REG_L);
			uint16_t bc = GetRegisterPair(REG_B, REG_C);
			int result = hl + bc;
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_CARRY, (result & 0x10000) != 0);
			SetFlag(FLAG_HALF_CARRY, ((hl ^ bc ^ (result & 0xFFFF)) & 0x1000) != 0);
			SetRegisterPair(REG_H, REG_L, static_cast<uint16_t>(result));
		}
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

		case 0x11: // LD DE,nn
			cycles += 12;
			SetRegisterPair(REG_D, REG_E, mmu.Read2Bytes(pc_));
			pc_ += 2;
			break;

		case 0x12: // LD (DE),A
			cycles += 8;
			mmu.WriteByte(GetRegisterPair(REG_D, REG_E), REG_A);
			break;

		case 0x13: // INC DE
			cycles += 8;
			SetRegisterPair(REG_D, REG_E, GetRegisterPair(REG_D, REG_E) + 1);
			break;

		case 0x14: // INC B
			cycles += 4;
			REG_B++;
			SetFlag(FLAG_ZERO, REG_B == 0);
			SetFlag(FLAG_ADD_SUB, true);
			SetFlag(FLAG_HALF_CARRY, (REG_B & 0x0F) == 0x0F);
			break;

		case 0x15: // DEC B
			cycles += 4;
			REG_B--;
			SetFlag(FLAG_ZERO, REG_B == 0);
			SetFlag(FLAG_ADD_SUB, true);
			SetFlag(FLAG_HALF_CARRY, (REG_B & 0x0F) == 0x0F);
			break;

		case 0x16: // LD D,n
			cycles += 8;
			REG_D = mmu.ReadByte(pc_++);
			break;

		case 0x18: // JR n
		{
			cycles += 12;
			int8_t jmpAmt = static_cast<int8_t>(mmu.ReadByte(pc_++));
			pc_ += jmpAmt;
		}
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

		case 0x1a: // LD A,(DE)
		{
			uint8_t val = mmu.ReadByte(GetRegisterPair(REG_D, REG_E));
			SetRegister(REG_A, val);
			cycles += 8;
		}
		break;

		case 0x1d: // DEC E
			cycles += 4;
			REG_E--;
			SetFlag(FLAG_ZERO, REG_E == 0);
			SetFlag(FLAG_ADD_SUB, true);
			SetFlag(FLAG_HALF_CARRY, (REG_E & 0x0F) == 0x0F);
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

		case 0x22: // LD (HL+),A
		{
			cycles += 8;
			uint16_t hl = GetRegisterPair(REG_H, REG_L);
			mmu.WriteByte(hl, REG_A);
			SetRegisterPair(REG_H, REG_L, hl + 1);
		}
		break;

		case 0x23: // INC HL
			cycles += 8;
			SetRegisterPair(REG_H, REG_L, GetRegisterPair(REG_H, REG_L) + 1);
			break;

		case 0x27: // DAA
			cycles += 4;

			if (!CheckFlag(FLAG_ADD_SUB)) {
				if (CheckFlag(FLAG_CARRY) || REG_A > 0x99)
				{
					REG_A += 0x60;
					SetFlag(FLAG_CARRY, true);
				}

				if (CheckFlag(FLAG_HALF_CARRY) || (REG_A & 0x0F) > 0x09)
				{
					REG_A += 0x6;
				}
			}
			else
			{
				if (CheckFlag(FLAG_CARRY))
					REG_A -= 0x60;
				if (CheckFlag(FLAG_HALF_CARRY))
					REG_A -= 0x6;
			}

			SetFlag(FLAG_ZERO, REG_A == 0);
			SetFlag(FLAG_HALF_CARRY, false);

			break;

		case 0x28: // JR Z,r8
			if (CheckFlag(FLAG_ZERO))
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

		case 0x2A: // LD A,(HL+)
		{
			cycles += 8;
			uint16_t hl = GetRegisterPair(REG_H, REG_L);
			SetRegister(REG_A, mmu.ReadByte(hl));
			SetRegisterPair(REG_H, REG_L, hl + 1);
		}
		break;

		case 0x2B: // DEC HL
			cycles += 8;
			SetRegisterPair(REG_H, REG_L, GetRegisterPair(REG_H, REG_L) - 1);
			break;

		case 0x2C: // INC L
			cycles += 4;
			REG_L++;
			SetFlag(FLAG_ZERO, REG_L == 0);
			SetFlag(FLAG_ADD_SUB, true);
			SetFlag(FLAG_HALF_CARRY, (REG_L & 0x0F) == 0x0F);
			break;

		case 0x2D: // DEC L
			cycles += 4;
			REG_L--;
			SetFlag(FLAG_ZERO, REG_L == 0);
			SetFlag(FLAG_ADD_SUB, true);
			SetFlag(FLAG_HALF_CARRY, (REG_L & 0x0F) == 0x0F);
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

		case 0x34: // INC (HL)
		{
			cycles += 12;
			uint16_t hl = GetRegisterPair(REG_H, REG_L);

			uint8_t val = mmu.ReadByte(hl);
			uint8_t result = val + 1;

			SetFlag(FLAG_ZERO, result == 0);
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_HALF_CARRY, (result & 0x0F) == 0x0F);

			mmu.WriteByte(hl, result);
		}
		break;

		case 0x36: // LD (HL), n
			cycles += 12;
			mmu.WriteByte(GetRegisterPair(REG_H, REG_L), mmu.ReadByte(pc_++));
			break;

		case 0x3c: // INC A
			cycles += 4;
			REG_A++;
			SetFlag(FLAG_ZERO, REG_A == 0);
			SetFlag(FLAG_ADD_SUB, true);
			SetFlag(FLAG_HALF_CARRY, (REG_A & 0x0F) == 0x0F);
			break;

		case 0x3d: // DEC A
			cycles += 4;
			REG_A--;
			SetFlag(FLAG_ZERO, REG_A == 0);
			SetFlag(FLAG_ADD_SUB, true);
			SetFlag(FLAG_HALF_CARRY, (REG_A & 0x0F) == 0x0F);
			break;

		case 0x3E: // LD A,#
			cycles += 8;
			SetRegister(REG_A, mmu.ReadByte(pc_++));
			break;

		case 0x40: // LD B,B
			cycles += 4;
			REG_B = REG_B;
			break;

		case 0x41: // LD B,C
			cycles += 4;
			REG_B = REG_C;
			break;

		case 0x42: // LD B,D
			cycles += 4;
			REG_B = REG_D;
			break;

		case 0x43: // LD B,E
			cycles += 4;
			REG_B = REG_E;
			break;

		case 0x44: // LD B,H
			cycles += 4;
			REG_B = REG_H;
			break;

		case 0x45: // LD B,L
			cycles += 4;
			REG_B = REG_L;
			break;

		case 0x46: // LD B,(HL)
			cycles += 8;
			REG_B = mmu.ReadByte(GetRegisterPair(REG_H, REG_L));
			break;

		case 0x47: // LD B,A
			cycles += 4;
			REG_B = REG_A;
			break;

		case 0x48: // LD C,B
			cycles += 4;
			REG_C = REG_B;
			break;

		case 0x49: // LD C,C
			cycles += 4;
			REG_C = REG_C;
			break;

		case 0x4A: // LD C,D
			cycles += 4;
			REG_C = REG_D;
			break;

		case 0x4B: // LD C,E
			cycles += 4;
			REG_C = REG_E;
			break;

		case 0x4C: // LD C,H
			cycles += 4;
			REG_C = REG_H;
			break;

		case 0x4D: // LD C,L
			cycles += 4;
			REG_C = REG_L;
			break;

		case 0x4E: // LD C,(HL)
			cycles += 8;
			REG_C = mmu.ReadByte(GetRegisterPair(REG_H, REG_L));
			break;

		case 0x4F: // LD C,A
			cycles += 4;
			REG_C = REG_A;
			break;

		case 0x50: // LD D,B
			cycles += 4;
			REG_D = REG_B;
			break;

		case 0x51: // LD D,C
			cycles += 4;
			REG_D = REG_C;
			break;

		case 0x52: // LD D,D
			cycles += 4;
			REG_D = REG_D;
			break;

		case 0x53: // LD D,E
			cycles += 4;
			REG_D = REG_E;
			break;

		case 0x54: // LD D,H
			cycles += 4;
			REG_D = REG_H;
			break;

		case 0x55: // LD D,L
			cycles += 4;
			REG_D = REG_L;
			break;

		case 0x56: // LD D,(HL)
			cycles += 8;
			REG_D = mmu.ReadByte(GetRegisterPair(REG_H, REG_L));
			break;

		case 0x57: // LD D,A
			cycles += 4;
			REG_D = REG_A;
			break;

		case 0x58: // LD E,B
			cycles += 4;
			REG_E = REG_B;
			break;

		case 0x59: // LD E,C
			cycles += 4;
			REG_E = REG_C;
			break;

		case 0x5A: // LD E,D
			cycles += 4;
			REG_E = REG_D;
			break;

		case 0x5B: // LD E,E
			cycles += 4;
			REG_E = REG_E;
			break;

		case 0x5C: // LD E,H
			cycles += 4;
			REG_E = REG_H;
			break;

		case 0x5D: // LD E,L
			cycles += 4;
			REG_E = REG_L;
			break;

		case 0x5E: // LD E,(HL)
			cycles += 8;
			REG_E = mmu.ReadByte(GetRegisterPair(REG_H, REG_L));
			break;

		case 0x5F: // LD E,A
			cycles += 4;
			REG_E = REG_A;
			break;

		case 0x60: // LH H,B
			cycles += 4;
			REG_H = REG_B;
			break;

		case 0x61: // LH H,C
			cycles += 4;
			REG_H = REG_C;
			break;

		case 0x62: // LH H,D
			cycles += 4;
			REG_H = REG_D;
			break;

		case 0x63: // LH H,E
			cycles += 4;
			REG_H = REG_E;
			break;

		case 0x64: // LH H,H
			cycles += 4;
			REG_H = REG_H;
			break;

		case 0x65: // LH H,L
			cycles += 4;
			REG_H = REG_L;
			break;

		case 0x66: // LD H,(HL)
		{
			cycles += 8;
			uint16_t hl = GetRegisterPair(REG_H, REG_L);
			REG_H = mmu.ReadByte(hl);
		}
		break;

		case 0x67: // LD H,A
			cycles += 4;
			REG_H = REG_A;
			break;
			
		case 0x68: // LD L,B
			cycles += 4;
			REG_L = REG_B;
			break;

		case 0x69: // LD L,C
			cycles += 4;
			REG_L = REG_C;
			break;

		case 0x6A: // LD L,D
			cycles += 4;
			REG_L = REG_D;
			break;

		case 0x6B: // LD L,E
			cycles += 4;
			REG_L = REG_E;
			break;

		case 0x6C: // LD L,H
			cycles += 4;
			REG_L = REG_H;
			break;

		case 0x6D: // LD L,L
			cycles += 4;
			REG_L = REG_L;
			break;

		case 0x6E: // LD L,(HL)
		{
			cycles += 8;
			uint16_t hl = GetRegisterPair(REG_H, REG_L);
			REG_L = mmu.ReadByte(hl);
		}
		break;

		case 0x6F: // LD L,A
			cycles += 4;
			REG_L = REG_A;
			break;

		case 0x70: // LD (HL),B
			cycles += 8;
			mmu.WriteByte(GetRegisterPair(REG_H, REG_L), REG_B);
			break;

		case 0x71: // LD (HL),C
			cycles += 8;
			mmu.WriteByte(GetRegisterPair(REG_H, REG_L), REG_C);
			break;

		case 0x72: // LD (HL),D
			cycles += 8;
			mmu.WriteByte(GetRegisterPair(REG_H, REG_L), REG_D);
			break;

		case 0x73: // LD (HL),E
			cycles += 8;
			mmu.WriteByte(GetRegisterPair(REG_H, REG_L), REG_E);
			break;

		case 0x74: // LD (HL),H
			cycles += 8;
			mmu.WriteByte(GetRegisterPair(REG_H, REG_L), REG_H);
			break;

		case 0x75: // LD (HL),L
			cycles += 8;
			mmu.WriteByte(GetRegisterPair(REG_H, REG_L), REG_L);
			break;


		case 0x77: // LD (HL),A
			mmu.WriteByte(GetRegisterPair(REG_H, REG_L), REG_A);
			cycles += 8;
			break;

		case 0x78: // LD A,B
			cycles += 4;
			SetRegister(REG_A, REG_B);
			break;

		case 0x79: // LD A,C
			cycles += 4;
			REG_A = REG_C;
			break;

		case 0x7E: // LD A,(HL)
			cycles += 8;
			REG_A = mmu.ReadByte(GetRegisterPair(REG_H, REG_L));
			break;

		case 0x87: // ADD A,A
			cycles += 4;
			REG_A += REG_A;
			SetFlag(FLAG_ZERO, REG_A == 0);
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_HALF_CARRY, (REG_A & 0x0F) + (REG_A & 0x0F) > 0x0F);
			SetFlag(FLAG_CARRY, (((REG_A & 0x0FF) + (REG_A & 0x0FF)) & 0x100) != 0);
			break;

		case 0x95: // SUB L
		{
			cycles += 4;

			int result = REG_A - REG_L;

			SetFlag(FLAG_CARRY, (result & 0x10000) != 0);
			SetFlag(FLAG_ZERO, result == 0);
			SetFlag(FLAG_ADD_SUB, true);

			// TODO: Pretty sure this flag set could be implemented wrong?...
			SetFlag(FLAG_HALF_CARRY, ((REG_A ^ REG_L ^ (result & 0xFFFF)) & 0x1000) != 0);

			SetRegister(REG_A, static_cast<uint8_t>(result));

			break;
		}

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

		case 0xB7: // OR A
			cycles += 4;
			SetRegister(REG_A, REG_A | REG_A);
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

		case 0xC1: // POP BC
			cycles += 12;
			SetRegisterPair(REG_B, REG_C, mmu.Read2Bytes(sp_));
			sp_ += 2;
			break;

		case 0xC3: // Jump to a memory address
			cycles += 16;
			pc_ = mmu.Read2Bytes(pc_);
			break;

		case 0xC5: // PUSH BC
			cycles += 16;
			PushStack(mmu, GetRegisterPair(REG_B, REG_C));
			break;

		case 0xC6: // ADD A,d8
		{
			cycles += 8;
			uint8_t add = mmu.ReadByte(pc_++);

			int result = REG_A + add;

			SetFlag(FLAG_CARRY, (result & 0x10000) != 0);
			SetFlag(FLAG_ZERO, result == 0);
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_HALF_CARRY, ((REG_A ^ add ^ (result & 0xFFFF)) & 0x1000) != 0);

			SetRegister(REG_A, static_cast<uint8_t>(result));
		}
		break;

		case 0xC8: // RET Z
			if (CheckFlag(FLAG_ZERO))
			{
				cycles += 20;
				pc_ = mmu.Read2Bytes(sp_);
				sp_ += 2;
			}
			else
			{
				cycles += 8;
			}
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

			case 0x87: // RES 0,A
				cycles += 8;
				REG_A = REG_A & 0b11111110;
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

		case 0xCE: // ADC A,d8
		{
			cycles += 8;
			uint8_t add = mmu.ReadByte(pc_++);


			int result = REG_A + add;

			SetFlag(FLAG_CARRY, (result & 0x10000) != 0);
			if (CheckFlag(FLAG_CARRY))
				result++;

			SetFlag(FLAG_ZERO, result == 0);
			SetFlag(FLAG_ADD_SUB, false);
			SetFlag(FLAG_HALF_CARRY, ((REG_A ^ add ^ (result & 0xFFFF)) & 0x1000) != 0);

			SetRegister(REG_A, static_cast<uint8_t>(result));
		}
		break;

		case 0xD1: // POP DE
			cycles += 12;
			SetRegisterPair(REG_D, REG_E, mmu.Read2Bytes(sp_));
			sp_ += 2;
			break;

		case 0xD5: // PUSH DE
			cycles += 16;
			PushStack(mmu, GetRegisterPair(REG_D, REG_E));
			break;

		case 0xD9: // RETI
			pending_disable_interrupts_ = false;
			interrupt_handler.SetMasterEnable(true);

			cycles += 16;

			pc_ = mmu.Read2Bytes(sp_);
			sp_ += 2;

			break;

		case 0xE0: // LD ($FF00+n),A
			cycles += 12;
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

		case 0xE5: // PUSH HL
			cycles += 16;
			PushStack(mmu, GetRegisterPair(REG_H, REG_L));

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

		case 0xF1: // POP AF
		{
			cycles += 12;
			SetRegisterPair(REG_A, REG_F, mmu.Read2Bytes(sp_));
			sp_ += 2;

			uint16_t af = GetRegisterPair(REG_A, REG_F);

			// TODO: How are we supposed to set these flags? Pretty sure some of these are wrong...
			SetFlag(FLAG_ZERO, af == 0);
			SetFlag(FLAG_ADD_SUB, static_cast<int16_t>(af) > 0);
			SetFlag(FLAG_CARRY, (af & 0x10000) != 0);
			SetFlag(FLAG_HALF_CARRY, (((af & 0xFFFF)) & 0x1000) != 0);
		}
		break;

		case 0xF3: // DI
			cycles += 4;
			pending_disable_interrupts_ = true;
			break;

		case 0xF5: // PUSH AF
			cycles += 16;
			PushStack(mmu, GetRegisterPair(REG_A, REG_F));
			break;

		case 0xFA: // LD A,(a16)
			cycles += 16;
			SetRegister(REG_A, mmu.ReadByte(mmu.Read2Bytes(pc_)));
			pc_ += 2;
			break;

		case 0xFB: // EI
			cycles += 4;
			pending_enable_interrupts_ = true;
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
			system("pause");
			break;
		}

		return cycles;
	}

}