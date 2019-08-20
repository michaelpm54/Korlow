#include <algorithm>
#include "mmu.h"

MMU::~MMU()
{
	delete mMem;
}

void MMU::reset(uint8_t *biosData)
{
	mMem = new uint8_t[0x10000];
	if (biosData)
	{
		std::copy_n(biosData, 0x100, &mMem[0]);
	}
}

void MMU::setRom(uint8_t *romData, int romSize)
{
	std::copy_n(romData, romSize, &mMem[0]);
}

void MMU::setBios(uint8_t *biosData)
{
	mBios = biosData;
}

uint8_t MMU::read8(uint16_t addr)
{
	if (addr < 0x100)
	{
		if (mInBios)
		{
			return mBios[addr];
		}
	}

	if (addr == 0xFF44)
	{
		return 0x90;
	}

	return mMem[addr];
}

uint16_t MMU::read16(uint16_t addr)
{
	return uint16_t(read8(addr+1) << 8) + read8(addr);
}

void MMU::ioWrite8(uint16_t addr, uint8_t val)
{
	if (addr > 0xFF0F && addr < 0xFF40)
	{
		switch (addr & 0xFF)
		{
			case 0x10:
				// NR10 - Channel 1 (Tone & Sweep) Sweep register (R/W) [-TTTDNNN] T=Time,D=direction,N=Numberof shifts 
				break;
			case 0x11:
				// NR11 - Channel 1 (Tone & Sweep) Sound length/Wave pattern duty (R/W) [DDLLLLLL] L=Length D=Wave pattern Duty
				printf("SND: Channel 1 write: Length: %02X, Wave pattern duty: %02X\n", val & 0xC0, val & ~0xC0);
				break;
			case 0x12:
				// NR12 - Channel 1 (Tone & Sweep) Volume Envelope (R/W) [VVVVDNNN] C1 Volume / Direction 0=down / envelope Number (fade speed)
				printf("SND: Channel 1 write: Vol: %02X, Down: %02X, Fade speed: %02X\n", val & 0xF0, val & 0x8, val & 0x7);
				break;
			case 0x13:
				break;
			// case 0x15: // There is no FF15
				// break;
			case 0x14:
				break;
			case 0x16:
				break;
			case 0x17:
				break;
			case 0x18:
				break;
			case 0x19:
				break;
			case 0x1A:
				break;
			case 0x1B:
				break;
			case 0x1C:
				break;
			case 0x1D:
				break;
			case 0x1E:
				break;
			case 0x20:
				break;
			case 0x21:
				break;
			case 0x22:
				break;
			case 0x23:
				break;
			case 0x24:
				break;
			case 0x25:
				break;
			case 0x26:
				// NR52 - Sound on/off [A---4321] read Channel 1-4 status or write All channels on/off (1=on)
				printf("SND: All sound: %s, Channels: %02X\n", ((val & 0x80) ? "on" : "off"), val & 0xF);
				break;
			default:
				break;
		}
	}
	else if (addr == 0xFF40)
	{
		// LCDC - LCD Control (R/W)
		printf("LCD control write: %02X\n", val);
	}
	else if (addr == 0xFF42)
	{
		printf("SCY ? Tile Scroll Y\n");
	}
	else if (addr == 0xFF43)
	{
		printf("SCX ? Tile Scroll X\n");
	}
	else if (addr == 0xFF47)
	{
		// BGP - BG Palette Data (R/W) - Non CGB Mode Only
		printf("Palette write: %02X\n", val);
	}
	else
	{
		printf("io write %02x to %04X\n", val, addr);
	}
}

void MMU::write8(uint16_t addr, uint8_t val)
{
	if (addr > 0xFEFF && addr < 0xFF80)
	{
		ioWrite8(addr, val);
	}
	mMem[addr] = val;
}

void MMU::write16(uint16_t addr, uint16_t val)
{
	mMem[addr] = val & 0xFF;
	mMem[addr+1] = (val & 0xFF00) >> 8;
}

void MMU::switchToRom()
{
	mInBios = false;
}
