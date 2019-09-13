#include "gpu.h"
#include "mmu.h"
#include "memory_map.h"

void MMU::init()
{
	mem.reset(new uint8_t[0x10000]);
	mem[kIf] = 0xE0;
}

uint8_t MMU::read8(uint16_t addr)
{
	if (addr < kHeader)
	{
		if (inBios)
		{
			return bios[addr];
		}
	}

	return mem[addr];
}

uint16_t MMU::read16(uint16_t addr)
{
	return uint16_t(read8(addr+1) << 8) + read8(addr);
}

void MMU::io_write8(uint16_t addr, uint8_t val)
{
	if (addr == 0xFF01)
	{
		serialData.push_back(val);
		printf("\nSerial data:\n{\n%s\n}\n\n", serialData.c_str());
		or8(kIf, 0x4);
	}
	else if (addr == 0xFF02)
	{
		// Serial transfer control
	}
	else if (addr > 0xFF0F && addr < kLcdc)
	{
		switch (addr & 0xFF)
		{
			case 0x10:
				// NR10 - Channel 1 (Tone & Sweep) Sweep register (R/W) [-TTTDNNN] T=Time,D=direction,N=Numberof shifts 
				break;
			case 0x11:
				// NR11 - Channel 1 (Tone & Sweep) Sound length/Wave pattern duty (R/W) [DDLLLLLL] L=Length D=Wave pattern Duty
				// printf("SND: Channel 1 write: Length: %02X, Wave pattern duty: %02X\n", val & 0xC0, val & ~0xC0);
				break;
			case 0x12:
				// NR12 - Channel 1 (Tone & Sweep) Volume Envelope (R/W) [VVVVDNNN] C1 Volume / Direction 0=down / envelope Number (fade speed)
				// printf("SND: Channel 1 write: Vol: %02X, Down: %02X, Fade speed: %02X\n", val & 0xF0, val & 0x8, val & 0x7);
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
				// printf("SND: All sound: %s, Channels: %02X\n", ((val & 0x80) ? "on" : "off"), val & 0xF);
				break;
			default:
				break;
		}
	}
	else if (addr == kLcdc)
	{
		// printf("LCD control write: %02X\n", val);
	}
	else if (addr == kScy)
	{
		// printf("SCY: 0x%02X | %d\n", val, val);
	}
	else if (addr == kScx)
	{
		// printf("SCX: 0x%02X | %d\n", val, val);
	}
	else if (addr == kLy)
	{
		val = 0;
	}
	else if (addr == kBgPalette)
	{
		gpu->setBgPalette(val);
	}
	else
	{
		// printf("io write %02x to %04X\n", val, addr);
	}
	mem[addr] = val;
}

void MMU::oam_write8(uint16_t addr, uint8_t val)
{
	printf("OAM write { %04X = %02X }\n", addr, val);
}

void MMU::write8(uint16_t addr, uint8_t val)
{
	if (addr >= kOam && addr < kIo)
	{
		oam_write8(addr, val);
	}
	if (addr >= kIo && addr < kZeroPage)
	{
		io_write8(addr, val);
		return;
	}
	if (val)
	{
		if (addr >= kTileRamUnsigned && addr < kTileRamSigned)
		{
			// printf("BG Data 1 write { %04X, %02X }\n", addr, val);
		}
		else if (addr >= kTileRamSigned && addr < kBgMap0)
		{
			// printf("BG Data 2 write { %04X, %02X }\n", addr, val);
		}
		else if (addr >= kBgMap0 && addr < kBgMap1)
		{
			// printf("BG tile map write 1 { %04X, %02X }\n", addr, val);
		}
		else if (addr >= kBgMap1 && addr < kCartRam)
		{
			// printf("BG tile map write 2 { %04X, %02X }\n", addr, val);
		}
	}
	if (addr == kIe)
	{
		printf("Enabled interrupt ");
		if (val & 0x1)
		{
			printf("VBLANK");
		}
		if (val & 0x2)
		{
			printf("LCD STAT");
		}
		if (val & 0x4)
		{
			printf("TIMER");
		}
		if (val & 0x8)
		{
			printf("SERIAL");
		}
		if (val & 0x10)
		{
			printf("JOYPAD");
		}
		else
		{
			printf("None");
		}
		printf("\n");
	}
	mem[addr] = val;
}

void MMU::write16(uint16_t addr, uint16_t val)
{
	mem[addr] = val & 0xFF;
	mem[addr+1] = (val & 0xFF00) >> 8;
}

void MMU::or8(uint16_t addr, uint8_t val)
{
	write8(addr, read8(addr) | val);
}

void MMU::or16(uint16_t addr, uint16_t val)
{
	write16(addr, read16(addr) | val);
}

void MMU::and8(uint16_t addr, uint8_t val)
{
	write8(addr, read8(addr) & val);
}

void MMU::and16(uint16_t addr, uint16_t val)
{
	write16(addr, read16(addr) & val);
}
