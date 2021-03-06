// GameCube DSP interpreter
#include "pch.h"

namespace DSP
{
	DspInterpreter::DspInterpreter(DspCore* parent)
	{
		core = parent;
	}

	DspInterpreter::~DspInterpreter()
	{
	}

	#pragma region "Top Instructions"

	void DspInterpreter::ABS(AnalyzeInfo& info)
	{
		int n = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[n];
		core->regs.ac[n].sbits = SignExtend40(core->regs.ac[n].sbits) >= 0 ? core->regs.ac[n].sbits : -core->regs.ac[n].sbits;
		Flags(a, a, core->regs.ac[n]);
	}

	void DspInterpreter::ADD(AnalyzeInfo& info)
	{
		int d = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[d];
		DspLongAccumulator b;
		b.sbits = SignExtend40(core->regs.ac[1 - d].sbits);
		core->regs.ac[d].sbits = SignExtend40(core->regs.ac[d].sbits);
		core->regs.ac[d].sbits += SignExtend40(core->regs.ac[1 - d].sbits);
		Flags(a, b, core->regs.ac[d]);
	}

	void DspInterpreter::ADDARN(AnalyzeInfo& info)
	{
		core->regs.ar[info.paramBits[0]] += core->regs.ix[info.paramBits[1]];
	}

	void DspInterpreter::ADDAX(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.sbits = SignExtend40((int64_t)core->regs.ax[info.paramBits[1]].sbits);
		core->regs.ac[info.paramBits[0]].sbits = SignExtend40(core->regs.ac[info.paramBits[0]].sbits);
		core->regs.ac[info.paramBits[0]].sbits += SignExtend40((int64_t)core->regs.ax[info.paramBits[1]].sbits);
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::ADDAXL(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.sbits = SignExtend16(core->regs.ax[info.paramBits[1]].l);
		core->regs.ac[info.paramBits[0]].sbits = SignExtend40(core->regs.ac[info.paramBits[0]].sbits);
		core->regs.ac[info.paramBits[0]].sbits += SignExtend16(core->regs.ax[info.paramBits[1]].l);
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::ADDI(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.shm = (int32_t)(int16_t)info.ImmOperand.UnsignedShort;
		b.l = 0;
		core->regs.ac[info.paramBits[0]].shm += (int32_t)(int16_t)info.ImmOperand.UnsignedShort;
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::ADDIS(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.shm = (int32_t)(int16_t)info.ImmOperand.SignedByte;
		b.l = 0;
		core->regs.ac[info.paramBits[0]].shm += (int32_t)(int16_t)info.ImmOperand.SignedByte;
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::ADDP(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.sbits = core->PackProd();
		core->regs.ac[info.paramBits[0]].sbits += core->PackProd();
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	// Fixed error in original Duddie doc: addpaxz acD.m, ax1.[l|h]

	void DspInterpreter::ADDPAXZ(AnalyzeInfo& info)
	{
		DspLongAccumulator a, b, c;
		a.sbits = core->PackProd();
		b.shm = info.paramBits[1] ? (int32_t)(int16_t)core->regs.ax[1].h : (int32_t)(int16_t)core->regs.ax[1].l;
		b.l = 0;
		c.shm = a.shm + b.shm;
		c.l = 0;
		core->regs.ac[info.paramBits[0]] = c;
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::ADDR(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.sbits = SignExtend16(core->MoveFromReg(info.paramBits[1]));
		core->regs.ac[info.paramBits[0]].sbits = SignExtend40(core->regs.ac[info.paramBits[0]].sbits);
		core->regs.ac[info.paramBits[0]].sbits += SignExtend16(core->MoveFromReg(info.paramBits[1]));
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::ANDC(AnalyzeInfo& info)
	{
		int n = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[n];
		core->regs.ac[n].m &= core->regs.ac[1 - n].m;
		Flags(a, a, core->regs.ac[n]);
	}

	void DspInterpreter::TCLR(AnalyzeInfo& info)
	{
		core->regs.sr.ok = (core->regs.ac[info.paramBits[0]].m & info.ImmOperand.UnsignedShort) == 0;
	}

	void DspInterpreter::TSET(AnalyzeInfo& info)
	{
		core->regs.sr.ok = (core->regs.ac[info.paramBits[0]].m & info.ImmOperand.UnsignedShort) == info.ImmOperand.UnsignedShort;
	}

	void DspInterpreter::ANDI(AnalyzeInfo& info)
	{
		int n = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[n];
		core->regs.ac[n].m &= info.ImmOperand.UnsignedShort;
		Flags(a, a, core->regs.ac[n]);
	}

	void DspInterpreter::ANDR(AnalyzeInfo& info)
	{
		int d = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[d];
		core->regs.ac[d].m &= core->regs.ax[info.paramBits[1]].h;
		Flags(a, a, core->regs.ac[d]);
	}

	void DspInterpreter::ASL(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		core->regs.ac[info.paramBits[0]].sbits <<= info.ImmOperand.SignedByte;
		Flags(a, a, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::ASR(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		core->regs.ac[info.paramBits[0]].sbits = SignExtend40(core->regs.ac[info.paramBits[0]].sbits);
		core->regs.ac[info.paramBits[0]].sbits >>= -info.ImmOperand.SignedByte;
		Flags(a, a, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::ASR16(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		core->regs.ac[info.paramBits[0]].sbits = SignExtend40(core->regs.ac[info.paramBits[0]].sbits);
		core->regs.ac[info.paramBits[0]].sbits >>= 16;
		Flags(a, a, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::BLOOP(AnalyzeInfo& info)
	{
		if (core->MoveFromReg(info.paramBits[0]) != 0)
		{
			SetLoop(core->regs.pc + 2, info.ImmOperand.Address, core->MoveFromReg(info.paramBits[0]));
			core->regs.pc += 2;
		}
		else
		{
			core->regs.pc = info.ImmOperand.Address + 1;
		}
	}

	void DspInterpreter::BLOOPI(AnalyzeInfo& info)
	{
		if (info.ImmOperand.Byte != 0)
		{
			SetLoop(core->regs.pc + 2, info.ImmOperand2.Address, info.ImmOperand.Byte);
			core->regs.pc += 2;
		}
		else
		{
			core->regs.pc = info.ImmOperand2.Address + 1;
		}
	}

	void DspInterpreter::CALLcc(AnalyzeInfo& info)
	{
		if (Condition(info.cc))
		{
			core->regs.st[0].push_back(core->regs.pc + 2);
			core->regs.pc = info.ImmOperand.Address;
		}
		else
		{
			core->regs.pc += 2;
		}
	}

	void DspInterpreter::CALLR(AnalyzeInfo& info)
	{
		core->regs.st[0].push_back(core->regs.pc + 1);
		core->regs.pc = core->MoveFromReg(info.paramBits[0]);
	}

	void DspInterpreter::CLR(AnalyzeInfo& info)
	{
		int n = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[n];
		core->regs.ac[n].bits = 0;
		Flags(a, a, core->regs.ac[n]);
	}

	void DspInterpreter::CLRL(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		core->regs.ac[info.paramBits[0]].l = 0;
		Flags(a, a, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::CLRP(AnalyzeInfo& info)
	{
		core->regs.prod.l = 0;
		core->regs.prod.m1 = 0;
		core->regs.prod.h = 0;
		core->regs.prod.m2 = 0;
	}

	void DspInterpreter::CMP(AnalyzeInfo& info)
	{
		DspLongAccumulator a, b, c;
		a.sbits = SignExtend40(core->regs.ac[0].sbits);
		b.sbits = -SignExtend40(core->regs.ac[1].sbits);
		c.sbits = a.sbits + b.sbits;
		Flags(a, b, c);
	}

	void DspInterpreter::CMPI(AnalyzeInfo& info)
	{
		DspLongAccumulator a, b, c;
		a.hm = core->regs.ac[info.paramBits[0]].hm;
		a.l = 0;
		b.hm = (int32_t)(int16_t)info.ImmOperand.UnsignedShort;
		b.l = 0;
		b.sbits = -b.sbits;
		c.sbits = a.sbits + b.sbits;
		Flags(a, b, c);
	}

	void DspInterpreter::CMPIS(AnalyzeInfo& info)
	{
		DspLongAccumulator a, b, c;
		a.hm = core->regs.ac[info.paramBits[0]].hm;
		a.l = 0;
		b.hm = (int32_t)(int16_t)info.ImmOperand.SignedByte;
		b.l = 0;
		b.sbits = -b.sbits;
		c.sbits = a.sbits + b.sbits;
		Flags(a, b, c);
	}

	// Compares accumulator $acS.m with accumulator ax1.[l|h]

	void DspInterpreter::CMPAR(AnalyzeInfo& info)
	{
		DspLongAccumulator a, b, c;
		a.sbits = SignExtend16(core->regs.ac[info.paramBits[0]].m);
		b.sbits = info.paramBits[1] ? SignExtend16(core->regs.ax[1].h) : SignExtend16(core->regs.ax[1].l);
		b.sbits = -b.sbits;
		c.sbits = a.sbits + b.sbits;
		Flags(a, b, c);
	}

	void DspInterpreter::DAR(AnalyzeInfo& info)
	{
		core->regs.ar[info.paramBits[0]]--;
	}

	void DspInterpreter::DEC(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		core->regs.ac[info.paramBits[0]].sbits--;
		Flags(a, a, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::DECM(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		core->regs.ac[info.paramBits[0]].hm--;
		Flags(a, a, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::HALT(AnalyzeInfo& info)
	{
		core->Suspend();
	}

	void DspInterpreter::IAR(AnalyzeInfo& info)
	{
		core->regs.ar[info.paramBits[0]]++;
	}

	void DspInterpreter::INC(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		core->regs.ac[info.paramBits[0]].sbits++;
		Flags(a, a, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::INCM(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		core->regs.ac[info.paramBits[0]].hm++;
		Flags(a, a, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::IFcc(AnalyzeInfo& info)
	{
		if (Condition(info.cc))
		{
			core->regs.pc++;
		}
		else
		{
			core->regs.pc += 2;
		}
	}

	void DspInterpreter::ILRR(AnalyzeInfo& info)
	{
		core->regs.ac[info.paramBits[0]].m = core->ReadIMem(core->regs.ar[info.paramBits[1]]);
	}

	void DspInterpreter::ILRRD(AnalyzeInfo& info)
	{
		core->regs.ac[info.paramBits[0]].m = core->ReadIMem(core->regs.ar[info.paramBits[1]]);
		core->regs.ar[info.paramBits[1]]--;
	}

	void DspInterpreter::ILRRI(AnalyzeInfo& info)
	{
		core->regs.ac[info.paramBits[0]].m = core->ReadIMem(core->regs.ar[info.paramBits[1]]);
		core->regs.ar[info.paramBits[1]]++;
	}

	void DspInterpreter::ILRRN(AnalyzeInfo& info)
	{
		core->regs.ac[info.paramBits[0]].m = core->ReadIMem(core->regs.ar[info.paramBits[1]]);
		core->regs.ar[info.paramBits[1]] += core->regs.ix[info.paramBits[1]];
	}

	void DspInterpreter::Jcc(AnalyzeInfo& info)
	{
		if (Condition(info.cc))
		{
			core->regs.pc = info.ImmOperand.Address;
		}
		else
		{
			core->regs.pc += 2;
		}
	}

	void DspInterpreter::JMPR(AnalyzeInfo& info)
	{
		core->regs.pc = core->MoveFromReg(info.paramBits[0]);
	}

	void DspInterpreter::LOOP(AnalyzeInfo& info)
	{
		if (core->MoveFromReg(info.paramBits[0]) != 0)
		{
			SetLoop(core->regs.pc + 1, core->regs.pc + 1, core->MoveFromReg(info.paramBits[0]));
			core->regs.pc++;
		}
		else
		{
			core->regs.pc += 2;
		}
	}

	void DspInterpreter::LOOPI(AnalyzeInfo& info)
	{
		if (info.ImmOperand.Byte != 0)
		{
			SetLoop(core->regs.pc + 1, core->regs.pc + 1, info.ImmOperand.Byte);
			core->regs.pc++;
		}
		else
		{
			core->regs.pc += 2;
		}
	}

	void DspInterpreter::LR(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramBits[0], core->ReadDMem(info.ImmOperand.UnsignedShort));
	}

	void DspInterpreter::LRI(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramBits[0], info.ImmOperand.UnsignedShort);
	}

	void DspInterpreter::LRIS(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramBits[0], (uint16_t)(int16_t)info.ImmOperand.SignedByte);
	}

	void DspInterpreter::LRR(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramBits[0], core->ReadDMem(core->regs.ar[info.paramBits[1]]));
	}

	void DspInterpreter::LRRD(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramBits[0], core->ReadDMem(core->regs.ar[info.paramBits[1]]));
		core->regs.ar[info.paramBits[1]]--;
	}

	void DspInterpreter::LRRI(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramBits[0], core->ReadDMem(core->regs.ar[info.paramBits[1]]));
		core->regs.ar[info.paramBits[1]]++;
	}

	void DspInterpreter::LRRN(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramBits[0], core->ReadDMem(core->regs.ar[info.paramBits[1]]));
		core->regs.ar[info.paramBits[1]] += core->regs.ix[info.paramBits[1]];
	}

	void DspInterpreter::LRS(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramBits[0], core->ReadDMem(
			(core->regs.bank << 8) | (uint8_t)info.ImmOperand.Address));
	}

	void DspInterpreter::LSL(AnalyzeInfo& info)
	{
		int n = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[n];
		core->regs.ac[n].bits <<= info.ImmOperand.Byte;
		Flags(a, a, core->regs.ac[n]);
	}

	void DspInterpreter::LSL16(AnalyzeInfo& info)
	{
		int n = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[n];
		core->regs.ac[n].bits <<= 16;
		Flags(a, a, core->regs.ac[n]);
	}

	void DspInterpreter::LSR(AnalyzeInfo& info)
	{
		int n = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[n];
		core->regs.ac[n].bits >>= -info.ImmOperand.SignedByte;
		Flags(a, a, core->regs.ac[n]);
	}

	void DspInterpreter::LSR16(AnalyzeInfo& info)
	{
		int n = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[n];
		core->regs.ac[n].bits >>= 16;
		Flags(a, a, core->regs.ac[n]);
	}

	void DspInterpreter::M2(AnalyzeInfo& info)
	{
		core->regs.sr.am = 0;
	}

	void DspInterpreter::M0(AnalyzeInfo& info)
	{
		core->regs.sr.am = 1;
	}

	void DspInterpreter::CLR15(AnalyzeInfo& info)
	{
		core->regs.sr.su = 0;
	}

	void DspInterpreter::SET15(AnalyzeInfo& info)
	{
		core->regs.sr.su = 1;
	}

	void DspInterpreter::CLR40(AnalyzeInfo& info)
	{
		core->regs.sr.sxm = 0;
	}

	void DspInterpreter::SET40(AnalyzeInfo& info)
	{
		core->regs.sr.sxm = 1;
	}

	void DspInterpreter::MOV(AnalyzeInfo& info)
	{
		int d = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[d];
		DspLongAccumulator b = core->regs.ac[1 - d];
		core->regs.ac[d] = core->regs.ac[1 - d];
		Flags(a, b, core->regs.ac[d]);
	}

	void DspInterpreter::MOVAX(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.sbits = (int64_t)core->regs.ax[info.paramBits[1]].sbits;
		core->regs.ac[info.paramBits[0]].sbits = (int64_t)core->regs.ax[info.paramBits[1]].sbits;
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::MOVNP(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.sbits = -core->PackProd();
		core->regs.ac[info.paramBits[0]].sbits = -core->PackProd();
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::MOVP(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.sbits = core->PackProd();
		core->regs.ac[info.paramBits[0]].sbits = core->PackProd();
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::MOVPZ(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.sbits = core->PackProd();
		core->regs.ac[info.paramBits[0]].sbits = core->PackProd();
		core->regs.ac[info.paramBits[0]].l = 0;
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::MOVR(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.hm = (int32_t)(int16_t)core->MoveFromReg(info.paramBits[1]);
		b.l = 0;
		core->regs.ac[info.paramBits[0]].hm = (int32_t)(int16_t)core->MoveFromReg(info.paramBits[1]);
		core->regs.ac[info.paramBits[0]].l = 0;
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::MRR(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramBits[0], core->MoveFromReg(info.paramBits[1]));
	}

	void DspInterpreter::NEG(AnalyzeInfo& info)
	{
		int n = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[n];
		DspLongAccumulator b;
		b.sbits = -SignExtend40(core->regs.ac[n].sbits);
		core->regs.ac[n].sbits = -SignExtend40(core->regs.ac[n].sbits);
		Flags(a, b, core->regs.ac[n]);
	}

	void DspInterpreter::ORC(AnalyzeInfo& info)
	{
		int n = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[n];
		DspLongAccumulator b;
		b.sbits = core->regs.ac[1 - n].m;
		core->regs.ac[n].m |= core->regs.ac[1 - n].m;
		Flags(a, b, core->regs.ac[n]);
	}

	void DspInterpreter::ORI(AnalyzeInfo& info)
	{
		int n = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[n];
		DspLongAccumulator b;
		b.sbits = info.ImmOperand.UnsignedShort;
		core->regs.ac[n].m |= info.ImmOperand.UnsignedShort;
		Flags(a, b, core->regs.ac[n]);
	}

	void DspInterpreter::ORR(AnalyzeInfo& info)
	{
		int d = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[d];
		DspLongAccumulator b;
		b.sbits = core->regs.ax[info.paramBits[1]].h;
		core->regs.ac[d].m |= core->regs.ax[info.paramBits[1]].h;
		Flags(a, b, core->regs.ac[d]);
	}

	void DspInterpreter::RETcc(AnalyzeInfo& info)
	{
		if (Condition(info.cc))
		{
			core->regs.pc = core->regs.st[0].back();
			core->regs.st[0].pop_back();
		}
		else
		{
			core->regs.pc++;
		}
	}

	void DspInterpreter::RTI(AnalyzeInfo& info)
	{
		core->ReturnFromException();
	}

	void DspInterpreter::SBSET(AnalyzeInfo& info)
	{
		core->regs.sr.bits |= (1 << info.ImmOperand.Byte);
	}

	void DspInterpreter::SBCLR(AnalyzeInfo& info)
	{
		core->regs.sr.bits &= ~(1 << info.ImmOperand.Byte);
	}

	void DspInterpreter::SI(AnalyzeInfo& info)
	{
		core->WriteDMem(info.ImmOperand.Address, info.ImmOperand2.UnsignedShort);
	}

	void DspInterpreter::SR(AnalyzeInfo& info)
	{
		core->WriteDMem(info.ImmOperand.Address, core->MoveFromReg(info.paramBits[1]));
	}

	void DspInterpreter::SRR(AnalyzeInfo& info)
	{
		core->WriteDMem(core->regs.ar[info.paramBits[0]], core->MoveFromReg(info.paramBits[1]));
	}

	void DspInterpreter::SRRD(AnalyzeInfo& info)
	{
		core->WriteDMem(core->regs.ar[info.paramBits[0]], core->MoveFromReg(info.paramBits[1]));
		core->regs.ar[info.paramBits[0]]--;
	}

	void DspInterpreter::SRRI(AnalyzeInfo& info)
	{
		core->WriteDMem(core->regs.ar[info.paramBits[0]], core->MoveFromReg(info.paramBits[1]));
		core->regs.ar[info.paramBits[0]]++;
	}

	void DspInterpreter::SRRN(AnalyzeInfo& info)
	{
		core->WriteDMem(core->regs.ar[info.paramBits[0]], core->MoveFromReg(info.paramBits[1]));
		core->regs.ar[info.paramBits[0]] += core->regs.ix[info.paramBits[0]];
	}

	void DspInterpreter::SRS(AnalyzeInfo& info)
	{
		core->WriteDMem( (core->regs.bank << 8) | (uint8_t)info.ImmOperand.Address,
			core->MoveFromReg(info.paramBits[1]));
	}

	void DspInterpreter::SUB(AnalyzeInfo& info)
	{
		int d = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[d];
		DspLongAccumulator b;
		b.sbits = -SignExtend40(core->regs.ac[1 - d].sbits);
		core->regs.ac[d].sbits = SignExtend40(core->regs.ac[d].sbits);
		core->regs.ac[d].sbits -= SignExtend40(core->regs.ac[1 - d].sbits);
		Flags(a, b, core->regs.ac[d]);
	}

	void DspInterpreter::SUBAX(AnalyzeInfo& info)
	{
		int64_t ax = (int64_t)core->regs.ax[info.paramBits[1]].sbits;
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.sbits = -ax;
		core->regs.ac[info.paramBits[0]].sbits = SignExtend40(core->regs.ac[info.paramBits[0]].sbits);
		core->regs.ac[info.paramBits[0]].sbits -= ax;
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::SUBP(AnalyzeInfo& info)
	{
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.sbits = -core->PackProd();
		core->regs.ac[info.paramBits[0]].sbits -= core->PackProd();
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::SUBR(AnalyzeInfo& info)
	{
		int64_t reg = SignExtend16(core->MoveFromReg(info.paramBits[1]));
		DspLongAccumulator a = core->regs.ac[info.paramBits[0]];
		DspLongAccumulator b;
		b.sbits = -reg;
		core->regs.ac[info.paramBits[0]].sbits = SignExtend40(core->regs.ac[info.paramBits[0]].sbits);
		core->regs.ac[info.paramBits[0]].sbits -= reg;
		Flags(a, b, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::TST(AnalyzeInfo& info)
	{
		DspLongAccumulator zr = { 0 };
		Flags(zr, zr, core->regs.ac[info.paramBits[0]]);
	}

	void DspInterpreter::TSTAXH(AnalyzeInfo& info)
	{
		DspLongAccumulator zr = { 0 };
		DspLongAccumulator axh;
		axh.sbits = SignExtend16(core->regs.ax[info.paramBits[0]].h);
		Flags(zr, zr, axh);
	}

	void DspInterpreter::XORI(AnalyzeInfo& info)
	{
		int n = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[n];
		DspLongAccumulator b;
		b.sbits = info.ImmOperand.UnsignedShort;
		core->regs.ac[n].m ^= info.ImmOperand.UnsignedShort;
		Flags(a, b, core->regs.ac[n]);
	}

	void DspInterpreter::XORR(AnalyzeInfo& info)
	{
		int d = info.paramBits[0];
		DspLongAccumulator a = core->regs.ac[d];
		DspLongAccumulator b;
		b.sbits = core->regs.ax[info.paramBits[1]].h;
		core->regs.ac[d].m ^= core->regs.ax[info.paramBits[1]].h;
		Flags(a, b, core->regs.ac[d]);
	}

	#pragma endregion "Top Instructions"


	#pragma region "Multiplier Instructions"

	void DspInterpreter::MADD(AnalyzeInfo& info)
	{
		// Multiply low part $axS.l of secondary accumulator $axS by high part $axS.h of secondary accumulator $axS
		// (treat them both as signed) and add result to product register. 
		//Madd16x16
		core->PackProd(0);
	}

	void DspInterpreter::MADDC(AnalyzeInfo& info)
	{
		// Multiply middle part of accumulator $acS.m by high part of secondary accumulator $axT.h
		// (treat them both as signed) and add result to product register.
		//Madd32x16
		core->PackProd(0);
	}

	void DspInterpreter::MADDX(AnalyzeInfo& info)
	{
		//Multiply one part of secondary accumulator $ax0 (selected by S) by one part of secondary accumulator $ax1 (selected by T)
		// (treat them both as signed) and add result to product register. 
		//Madd16x16
		core->PackProd(0);
	}

	void DspInterpreter::MSUB(AnalyzeInfo& info)
	{
		//Multiply low part $axS.l of secondary accumulator $axS by high part $axS.h of secondary accumulator $axS 
		//(treat them both as signed) and subtract result from product register. 
		//Msub16x16
		core->PackProd(0);
	}

	void DspInterpreter::MSUBC(AnalyzeInfo& info)
	{	
		//Multiply middle part of accumulator $acS.m by high part of secondary accumulator $axT.h
		// (treat them both as signed) and subtract result from product register.
		//Msub32x16
		core->PackProd(0);
	}

	void DspInterpreter::MSUBX(AnalyzeInfo& info)
	{
		//Multiply one part of secondary accumulator $ax0 (selected by S) by one part of secondary accumulator $ax1 (selected by T) 
		// (treat them both as signed) and subtract result from product register. 
		//Msub16x16
		core->PackProd(0);
	}

	void DspInterpreter::MUL(AnalyzeInfo& info)
	{
		//Multiply low part $axS.l of secondary accumulator $axS by high part $axS.h of secondary accumulator $axS (treat them both as signed). 
		//Mul16x16
		core->PackProd(0);
	}

	void DspInterpreter::MULAC(AnalyzeInfo& info)
	{
		// Add product register to accumulator register $acR.
		// Multiply low part $axS.l of secondary accumulator $axS by high part $axS.h of secondary accumulator $axS(treat them both as signed).
		//Mul16x16
		core->PackProd(0);
	}

	void DspInterpreter::MULC(AnalyzeInfo& info)
	{
		//Multiply mid part of accumulator register $acS.m by high part $axS.h of secondary accumulator $axS (treat them both as signed).
		//Mul32x16
		core->PackProd(0);
	}

	void DspInterpreter::MULCAC(AnalyzeInfo& info)
	{
		// Multiply mid part of accumulator register $acS.m by high part $axS.h of secondary accumulator $axS(treat them both as signed).
		// Add product register before multiplication to accumulator $acR.
		//Mul32x16
		core->PackProd(0);
	}

	void DspInterpreter::MULCMV(AnalyzeInfo& info)
	{
		// Multiply mid part of accumulator register $acS.m by high part $axS.h of secondary accumulator $axS(treat them both as signed).
		// Move product register before multiplication to accumulator $acR.
		//Mul32x16
		core->PackProd(0);
	}

	void DspInterpreter::MULCMVZ(AnalyzeInfo& info)
	{
		// Multiply mid part of accumulator register $acS.m by high part $axS.h of secondary accumulator $axS (treat them both as signed). 
		// Move product register before multiplication to accumulator $acR. Set low part of accumulator $acR.l to zero. 
		//Mul32x16
		core->PackProd(0);
	}

	void DspInterpreter::MULMV(AnalyzeInfo& info)
	{
		// Move product register to accumulator register $acR. Multiply low part $axS.l of secondary accumulator Register$axS by high part $axS.h of secondary accumulator $axS
		// (treat them both as signed). 
		//Mul16x16
		core->PackProd(0);
	}

	void DspInterpreter::MULMVZ(AnalyzeInfo& info)
	{
		// Move product register to accumulator register $acRand clear low part of accumulator register $acR.l.
		// Multiply low part $axS.l of secondary accumulator $axS by high part $axS.h of secondary accumulator $axS(treat them both as signed).
		//Mul16x16
		core->PackProd(0);
	}

	void DspInterpreter::MULX(AnalyzeInfo& info)
	{
		int64_t a = SignExtend16(info.paramBits[0] ? core->regs.ax[0].h : core->regs.ax[0].l);
		int64_t b = SignExtend16(info.paramBits[1] ? core->regs.ax[1].h : core->regs.ax[1].l);
		core->PackProd(a * b);
	}

	void DspInterpreter::MULXAC(AnalyzeInfo& info)
	{
		core->regs.ac[info.paramBits[2]].sbits += core->PackProd();
		int64_t a = SignExtend16(info.paramBits[0] ? core->regs.ax[0].h : core->regs.ax[0].l);
		int64_t b = SignExtend16(info.paramBits[1] ? core->regs.ax[1].h : core->regs.ax[1].l);
		core->PackProd(a * b);
	}

	void DspInterpreter::MULXMV(AnalyzeInfo& info)
	{
		core->regs.ac[info.paramBits[2]].sbits = core->PackProd();
		int64_t a = SignExtend16(info.paramBits[0] ? core->regs.ax[0].h : core->regs.ax[0].l);
		int64_t b = SignExtend16(info.paramBits[1] ? core->regs.ax[1].h : core->regs.ax[1].l);
		core->PackProd(a * b);
	}

	void DspInterpreter::MULXMVZ(AnalyzeInfo& info)
	{
		//Move product register to accumulator register $acR and clear low part of accumulator register $acR.l. 
		//Multiply one part $ax0 by one part $ax1 (treat them both as signed).
		//Part is selected by S and T bits. Zero selects low part, one selects high part.
		//Mul16x16
		core->PackProd(0);
	}

	#pragma region "Multiplier Instructions"


	#pragma region "Bottom Instructions"

	void DspInterpreter::DR(AnalyzeInfo& info)
	{
		core->regs.ar[info.paramExBits[0]]--;
	}

	void DspInterpreter::IR(AnalyzeInfo& info)
	{
		core->regs.ar[info.paramExBits[0]]++;
	}

	void DspInterpreter::NR(AnalyzeInfo& info)
	{
		core->regs.ar[info.paramExBits[0]] += core->regs.ix[info.paramExBits[0]];
	}

	void DspInterpreter::MV(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramExBits[0], core->MoveFromReg(info.paramExBits[1]));
	}

	void DspInterpreter::S(AnalyzeInfo& info)
	{
		core->WriteDMem(core->regs.ar[info.paramExBits[0]], core->MoveFromReg(info.paramExBits[1]));
		core->regs.ar[info.paramExBits[0]]++;
	}

	void DspInterpreter::SN(AnalyzeInfo& info)
	{
		core->WriteDMem(core->regs.ar[info.paramExBits[0]], core->MoveFromReg(info.paramExBits[1]));
		core->regs.ar[info.paramExBits[0]] += core->regs.ix[info.paramExBits[0]];
	}

	void DspInterpreter::L(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramExBits[0], core->ReadDMem(core->regs.ar[info.paramExBits[1]]));
		core->regs.ar[info.paramExBits[1]]++;
	}

	void DspInterpreter::LN(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramExBits[0], core->ReadDMem(core->regs.ar[info.paramExBits[1]]));
		core->regs.ar[info.paramExBits[1]] += core->regs.ix[info.paramExBits[1]];
	}

	void DspInterpreter::LS(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramExBits[0], core->ReadDMem(core->regs.ar[0]));
		core->WriteDMem(core->regs.ar[3], core->regs.ac[info.paramExBits[1]].m);
		core->regs.ar[0]++;
		core->regs.ar[3]++;
	}

	void DspInterpreter::SL(AnalyzeInfo& info)
	{
		core->WriteDMem(core->regs.ar[0], core->regs.ac[info.paramExBits[0]].m);
		core->MoveToReg(info.paramExBits[1], core->ReadDMem(core->regs.ar[3]));
		core->regs.ar[0]++;
		core->regs.ar[3]++;
	}

	void DspInterpreter::LSN(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramExBits[0], core->ReadDMem(core->regs.ar[0]));
		core->WriteDMem(core->regs.ar[3], core->regs.ac[info.paramExBits[1]].m);
		core->regs.ar[0] += core->regs.ix[0];
		core->regs.ar[3]++;
	}

	void DspInterpreter::SLN(AnalyzeInfo& info)
	{
		core->WriteDMem(core->regs.ar[0], core->regs.ac[info.paramExBits[0]].m);
		core->MoveToReg(info.paramExBits[1], core->ReadDMem(core->regs.ar[3]));
		core->regs.ar[0] += core->regs.ix[0];
		core->regs.ar[3]++;
	}

	void DspInterpreter::LSM(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramExBits[0], core->ReadDMem(core->regs.ar[0]));
		core->WriteDMem(core->regs.ar[3], core->regs.ac[info.paramExBits[1]].m);
		core->regs.ar[0]++;
		core->regs.ar[3] += core->regs.ix[3];
	}

	void DspInterpreter::SLM(AnalyzeInfo& info)
	{
		core->WriteDMem(core->regs.ar[0], core->regs.ac[info.paramExBits[0]].m);
		core->MoveToReg(info.paramExBits[1], core->ReadDMem(core->regs.ar[3]));
		core->regs.ar[0]++;
		core->regs.ar[3] += core->regs.ix[3];
	}

	void DspInterpreter::LSNM(AnalyzeInfo& info)
	{
		core->MoveToReg(info.paramExBits[0], core->ReadDMem(core->regs.ar[0]));
		core->WriteDMem(core->regs.ar[3], core->regs.ac[info.paramExBits[1]].m);
		core->regs.ar[0] += core->regs.ix[0];
		core->regs.ar[3] += core->regs.ix[3];
	}

	void DspInterpreter::SLNM(AnalyzeInfo& info)
	{
		core->WriteDMem(core->regs.ar[0], core->regs.ac[info.paramExBits[0]].m);
		core->MoveToReg(info.paramExBits[1], core->ReadDMem(core->regs.ar[3]));
		core->regs.ar[0] += core->regs.ix[0];
		core->regs.ar[3] += core->regs.ix[3];
	}

	// TODO: The last 2 groups of opcodes look very crazy. Need to reverse UCodes and understand in context how they actually work.

	// Подозрительно похоже что вместо ar3 на самом деле используется ar[S] и ar[3-S], как мы это видели в опкодах ранее.
	// Поэтому на ar[S] и ar[3] в примерах показанные как ar[0] и ar[3] на самом деле будут ar[0] и ar[3 - 0].
	// Suspiciously, instead of ar3 - ar[S] and ar[3-S] are actually used, as we saw in the opcodes earlier.
	// Therefore, on ar[S] and ar[3] in the examples shown as ar[0] and ar[3] will actually be ar[0] and ar[3-0].

	// LD $ax0.d, $ax1.r, @$arS 
	// ax0.d (d = Low/High) = *arS;  ax1.r (r = Low/High) = *ar3
	// Postincrement arS, ar3

	void DspInterpreter::LDCommon(AnalyzeInfo& info)
	{
		if (info.paramExBits[0])
		{
			core->regs.ax[0].l = core->ReadDMem(core->regs.ar[info.paramExBits[2]]);		// S
		}
		else
		{
			core->regs.ax[0].h = core->ReadDMem(core->regs.ar[info.paramExBits[2]]);		// S
		}

		if (info.paramExBits[1])
		{
			core->regs.ax[1].l = core->ReadDMem(core->regs.ar[3]);		// 3 - S  ???
		}
		else
		{
			core->regs.ax[1].h = core->ReadDMem(core->regs.ar[3]);		// 3 - S ???
		}
	}

	void DspInterpreter::LD(AnalyzeInfo& info)
	{
		LDCommon(info);
		core->regs.ar[info.paramExBits[2]]++;
		core->regs.ar[3]++;
	}

	void DspInterpreter::LDN(AnalyzeInfo& info)
	{
		LDCommon(info);
		core->regs.ar[info.paramExBits[2]] += core->regs.ix[info.paramExBits[2]];
		core->regs.ar[3]++;
	}

	void DspInterpreter::LDM(AnalyzeInfo& info)
	{
		LDCommon(info);
		core->regs.ar[info.paramExBits[2]]++;
		core->regs.ar[3] += core->regs.ix[3];
	}

	void DspInterpreter::LDNM(AnalyzeInfo& info)
	{
		LDCommon(info);
		core->regs.ar[info.paramExBits[2]] += core->regs.ix[info.paramExBits[2]];
		core->regs.ar[3] += core->regs.ix[3];
	}

	// LDAX $axR, @$arS
	// axR.h = *arS;  axR.l = *ar3;
	// Postincrement arS, ar3

	void DspInterpreter::LDAXCommon(AnalyzeInfo& info)
	{
		core->regs.ax[info.paramExBits[0]].h = core->ReadDMem(core->regs.ar[info.paramExBits[1]]);		// S
		core->regs.ax[info.paramExBits[0]].l = core->ReadDMem(core->regs.ar[3]);		// 3 - S ???
	}

	void DspInterpreter::LDAX(AnalyzeInfo& info)
	{
		LDAXCommon(info);
		core->regs.ar[info.paramExBits[1]]++;
		core->regs.ar[3]++;
	}

	void DspInterpreter::LDAXN(AnalyzeInfo& info)
	{
		LDAXCommon(info);
		core->regs.ar[info.paramExBits[1]] += core->regs.ix[info.paramExBits[1]];
		core->regs.ar[3]++;
	}

	void DspInterpreter::LDAXM(AnalyzeInfo& info)
	{
		LDAXCommon(info);
		core->regs.ar[info.paramExBits[1]]++;
		core->regs.ar[3] += core->regs.ix[3];
	}

	void DspInterpreter::LDAXNM(AnalyzeInfo& info)
	{
		LDAXCommon(info);
		core->regs.ar[info.paramExBits[1]] += core->regs.ix[info.paramExBits[1]];
		core->regs.ar[3] += core->regs.ix[3];
	}

	#pragma endregion "Bottom Instructions"

	int64_t DspInterpreter::SignExtend40(int64_t a)
	{
		if (a & 0x8000000000)
		{
			a |= 0xffffff0000000000;
		}
		return a;
	}

	int64_t DspInterpreter::SignExtend16(int16_t a)
	{
		int64_t res = a;
		if (res & 0x8000)
		{
			res |= 0xffffffffffff0000;
		}
		return res;
	}

	bool DspInterpreter::Condition(ConditionCode cc)
	{
		switch (cc)
		{
			case ConditionCode::GE: return core->regs.sr.s == core->regs.sr.o;
			case ConditionCode::L: return core->regs.sr.s != core->regs.sr.o;
			case ConditionCode::G: return (core->regs.sr.s == core->regs.sr.o) && (core->regs.sr.z == 0);
			case ConditionCode::LE: return (core->regs.sr.s != core->regs.sr.o) && (core->regs.sr.z != 0);
			case ConditionCode::NE: return core->regs.sr.z == 0;
			case ConditionCode::EQ: return core->regs.sr.z != 0;
			case ConditionCode::NC: return core->regs.sr.c == 0;
			case ConditionCode::C: return core->regs.sr.c != 0;
			case ConditionCode::BelowS32: return core->regs.sr.as == 0;
			case ConditionCode::AboveS32: return core->regs.sr.as != 0;
			case ConditionCode::NOK: return core->regs.sr.ok == 0;
			case ConditionCode::OK: return core->regs.sr.ok != 0;
			case ConditionCode::O: return core->regs.sr.o != 0;
			case ConditionCode::Always: return true;
		}

		return false;
	}

	// 32-bit accumulator does not involved in flags calculations

	void DspInterpreter::Flags40(int64_t a, int64_t b, int64_t res)
	{
		bool carry = (res & 0x10000000000) != 0;
		bool zero = (res & 0xffffffffff) == 0;
		bool aboveS32 = (res & 0xff00000000) == 0;		// Correct?
		bool msb = (res & 0x8000000000) != 0 ? true : false;
		bool afterMsb = (res & 0x4000000000) != 0 ? true : false;
		// http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt
		//1. If the sum of two numbers with the sign bits off yields a result number
		//   with the sign bit on, the "overflow" flag is turned on.
		//   0100 + 0100 = 1000 (overflow flag is turned on)
		//2. If the sum of two numbers with the sign bits on yields a result number
		//   with the sign bit off, the "overflow" flag is turned on.
		//   1000 + 1000 = 0000 (overflow flag is turned on)
		//A human need only remember that, when doing signed math, adding
		//two numbers of the same sign must produce a result of the same sign,
		//otherwise overflow happened.
		bool aMsb = (a & 0x8000000000) != 0 ? true : false;
		bool bMsb = (b & 0x8000000000) != 0 ? true : false;
		bool ovf = false;
		if (aMsb == bMsb)
		{
			ovf = aMsb != msb;
		}

		core->regs.sr.c = carry;
		core->regs.sr.o = ovf;
		core->regs.sr.z = zero;
		core->regs.sr.s = msb;
		core->regs.sr.tt = msb == afterMsb;
		core->regs.sr.as = aboveS32;
	}

	void DspInterpreter::Flags(DspLongAccumulator a, DspLongAccumulator b, DspLongAccumulator res)
	{
		Flags40(SignExtend40(a.bits), SignExtend40(b.bits), SignExtend40(res.bits));
	}

	void DspInterpreter::SetLoop(DspAddress startAddr, DspAddress endAddr, uint16_t count)
	{
		core->regs.st[0].push_back(startAddr);
		core->regs.st[2].push_back(endAddr);
		core->regs.st[3].push_back(count);
	}

	bool DspInterpreter::CheckLoop()
	{
		while (!core->regs.st[3].empty())
		{
			if (core->regs.st[3].back() != 0)
			{
				if (core->regs.pc == core->regs.st[2].back())
				{
					core->regs.pc = core->regs.st[0].back();
					core->regs.st[3].back()--;
					return true;
				}
				break;
			}
			else
			{
				core->regs.st[0].pop_back();
				core->regs.st[2].pop_back();
				core->regs.st[3].pop_back();
			}
		}

		return false;
	}

	void DspInterpreter::Dispatch(AnalyzeInfo& info)
	{
		// Test breakpoints and canaries
		if (core->IsRunning())
		{
			if (core->TestBreakpoint(core->regs.pc))
			{
				DBHalt("DSP: IMEM breakpoint at 0x%04X\n", core->regs.pc);
				core->Suspend();
				return;
			}

			core->TestCanary(core->regs.pc);
		}

		// Regular instructions ("top")
		switch (info.instr)
		{
			case DspInstruction::ABS: ABS(info); break;

			case DspInstruction::ADD: ADD(info); break;
			case DspInstruction::ADDARN: ADDARN(info); break;
			case DspInstruction::ADDAX: ADDAX(info); break;
			case DspInstruction::ADDAXL: ADDAXL(info); break;
			case DspInstruction::ADDI: ADDI(info); break;
			case DspInstruction::ADDIS: ADDIS(info); break;
			case DspInstruction::ADDP: ADDP(info); break;
			case DspInstruction::ADDPAXZ: ADDPAXZ(info); break;
			case DspInstruction::ADDR: ADDR(info); break;

			case DspInstruction::ANDC: ANDC(info); break;
			case DspInstruction::TCLR: TCLR(info); break;
			case DspInstruction::TSET: TSET(info); break;
			case DspInstruction::ANDI: ANDI(info); break;
			case DspInstruction::ANDR: ANDR(info); break;

			case DspInstruction::ASL: ASL(info); break;
			case DspInstruction::ASR: ASR(info); break;
			case DspInstruction::ASR16: ASR16(info); break;

			case DspInstruction::BLOOP: BLOOP(info); break;
			case DspInstruction::BLOOPI: BLOOPI(info); break;
			case DspInstruction::CALLcc: CALLcc(info); break;
			case DspInstruction::CALLR: CALLR(info); break;

			case DspInstruction::CLR: CLR(info); break;
			case DspInstruction::CLRL: CLRL(info); break;
			case DspInstruction::CLRP: CLRP(info); break;

			case DspInstruction::CMP: CMP(info); break;
			case DspInstruction::CMPI: CMPI(info); break;
			case DspInstruction::CMPIS: CMPIS(info); break;
			case DspInstruction::CMPAR: CMPAR(info); break;

			case DspInstruction::DAR: DAR(info); break;
			case DspInstruction::DEC: DEC(info); break;
			case DspInstruction::DECM: DECM(info); break;

			case DspInstruction::HALT: HALT(info); break;

			case DspInstruction::IAR: IAR(info); break;
			case DspInstruction::INC: INC(info); break;
			case DspInstruction::INCM: INCM(info); break;

			case DspInstruction::IFcc: IFcc(info); break;

			case DspInstruction::ILRR: ILRR(info); break;
			case DspInstruction::ILRRD: ILRRD(info); break;
			case DspInstruction::ILRRI: ILRRI(info); break;
			case DspInstruction::ILRRN: ILRRN(info); break;

			case DspInstruction::Jcc: Jcc(info); break;
			case DspInstruction::JMPR: JMPR(info); break;
			case DspInstruction::LOOP: LOOP(info); break;
			case DspInstruction::LOOPI: LOOPI(info); break;

			case DspInstruction::LR: LR(info); break;
			case DspInstruction::LRI: LRI(info); break;
			case DspInstruction::LRIS: LRIS(info); break;
			case DspInstruction::LRR: LRR(info); break;
			case DspInstruction::LRRD: LRRD(info); break;
			case DspInstruction::LRRI: LRRI(info); break;
			case DspInstruction::LRRN: LRRN(info); break;
			case DspInstruction::LRS: LRS(info); break;

			case DspInstruction::LSL: LSL(info); break;
			case DspInstruction::LSL16: LSL16(info); break;
			case DspInstruction::LSR: LSR(info); break;
			case DspInstruction::LSR16: LSR16(info); break;

			case DspInstruction::M2: M2(info); break;
			case DspInstruction::M0: M0(info); break;
			case DspInstruction::CLR15: CLR15(info); break;
			case DspInstruction::SET15: SET15(info); break;
			case DspInstruction::CLR40: CLR40(info); break;
			case DspInstruction::SET40: SET40(info); break;

			case DspInstruction::MOV: MOV(info); break;
			case DspInstruction::MOVAX: MOVAX(info); break;
			case DspInstruction::MOVNP: MOVNP(info); break;
			case DspInstruction::MOVP: MOVP(info); break;
			case DspInstruction::MOVPZ: MOVPZ(info); break;
			case DspInstruction::MOVR: MOVR(info); break;
			case DspInstruction::MRR: MRR(info); break;

			case DspInstruction::NEG: NEG(info); break;

			case DspInstruction::ORC: ORC(info); break;
			case DspInstruction::ORI: ORI(info); break;
			case DspInstruction::ORR: ORR(info); break;

			case DspInstruction::RETcc: RETcc(info); break;
			case DspInstruction::RTI: RTI(info); break;

			case DspInstruction::SBSET: SBSET(info); break;
			case DspInstruction::SBCLR: SBCLR(info); break;

			case DspInstruction::SI: SI(info); break;
			case DspInstruction::SR: SR(info); break;
			case DspInstruction::SRR: SRR(info); break;
			case DspInstruction::SRRD: SRRD(info); break;
			case DspInstruction::SRRI: SRRI(info); break;
			case DspInstruction::SRRN: SRRN(info); break;
			case DspInstruction::SRS: SRS(info); break;

			case DspInstruction::SUB: SUB(info); break;
			case DspInstruction::SUBAX: SUBAX(info); break;
			case DspInstruction::SUBP: SUBP(info); break;
			case DspInstruction::SUBR: SUBR(info); break;

			case DspInstruction::TST: TST(info); break;
			case DspInstruction::TSTAXH: TSTAXH(info); break;

			case DspInstruction::XORI: XORI(info); break;
			case DspInstruction::XORR: XORR(info); break;

			//case DspInstruction::MADD: MADD(info); break;
			//case DspInstruction::MADDC: MADDC(info); break;
			//case DspInstruction::MADDX: MADDX(info); break;
			//case DspInstruction::MSUB: MSUB(info); break;
			//case DspInstruction::MSUBC: MSUBC(info); break;
			//case DspInstruction::MSUBX: MSUBX(info); break;
			//case DspInstruction::MUL: MUL(info); break;
			//case DspInstruction::MULAC: MULAC(info); break;
			//case DspInstruction::MULC: MULC(info); break;
			//case DspInstruction::MULCAC: MULCAC(info); break;
			//case DspInstruction::MULCMV: MULCMV(info); break;
			//case DspInstruction::MULCMVZ: MULCMVZ(info); break;
			//case DspInstruction::MULMV: MULMV(info); break;
			//case DspInstruction::MULMVZ: MULMVZ(info); break;
			case DspInstruction::MULX: MULX(info); break;
			case DspInstruction::MULXAC: MULXAC(info); break;
			case DspInstruction::MULXMV: MULXMV(info); break;
			//case DspInstruction::MULXMVZ: MULXMVZ(info); break;

			case DspInstruction::NOP:
			case DspInstruction::NX:
				break;

			default:
				DBHalt("DSP Unknown instruction at 0x%04X\n", core->regs.pc);
				core->Suspend();
				return;
		}

		// Packed tuple ("bottom")
		if (info.extendedOpcodePresent)
		{
			switch (info.instrEx)
			{
				case DspInstructionEx::DR: DR(info); break;
				case DspInstructionEx::IR: IR(info); break;
				case DspInstructionEx::NR: NR(info); break;
				case DspInstructionEx::MV: MV(info); break;
				case DspInstructionEx::S: S(info); break;
				case DspInstructionEx::SN: SN(info); break;
				case DspInstructionEx::L: L(info); break;
				case DspInstructionEx::LN: LN(info); break;

				case DspInstructionEx::LS: LS(info); break;
				case DspInstructionEx::SL: SL(info); break;
				case DspInstructionEx::LSN: LSN(info); break;
				case DspInstructionEx::SLN: SLN(info); break;
				case DspInstructionEx::LSM: LSM(info); break;
				case DspInstructionEx::SLM: SLM(info); break;
				case DspInstructionEx::LSNM: LSNM(info); break;
				case DspInstructionEx::SLNM: SLNM(info); break;

				case DspInstructionEx::LD: LD(info); break;
				case DspInstructionEx::LDN: LDN(info); break;
				case DspInstructionEx::LDM: LDM(info); break;
				case DspInstructionEx::LDNM: LDNM(info); break;

				case DspInstructionEx::LDAX: LDAX(info); break;
				case DspInstructionEx::LDAXN: LDAXN(info); break;
				case DspInstructionEx::LDAXM: LDAXM(info); break;
				case DspInstructionEx::LDAXNM: LDAXNM(info); break;

				case DspInstructionEx::NOP2: break;

				default:
					DBHalt("DSP Unknown packed instruction at 0x%04X\n", core->regs.pc);
					core->Suspend();
					return;
			}
		}

		if (!info.flowControl && !CheckLoop())
		{
			core->regs.pc += (DspAddress)(info.sizeInBytes >> 1);
		}
	}

	void DspInterpreter::ExecuteInstr()
	{
		AnalyzeInfo info;

		// Fetch, analyze and dispatch instruction at pc addr

		DspAddress imemAddr = core->regs.pc;

		uint8_t* imemPtr = core->TranslateIMem(imemAddr);
		if (imemPtr == nullptr)
		{
			DBHalt("DSP TranslateIMem failed on dsp addr: 0x%04X\n", imemAddr);
			core->Suspend();
			return;
		}

		if (!Analyzer::Analyze(imemPtr, DspCore::MaxInstructionSizeInBytes, info))
		{
			DBHalt("DSP Analyzer failed on dsp addr: 0x%04X\n", imemAddr);
			core->Suspend();
			return;
		}

		Dispatch(info);
	}

}
