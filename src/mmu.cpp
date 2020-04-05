#include "mmu.h"
#include "memory_map.h"

bool is_ppu_address(uint16_t address)
{
	return
		(address >= kOam && address < kIo)
		|| (address >= kTileRamUnsigned && address < kCartRam)
		|| (address >= kLcdc && address < kZeroPage);
}

bool is_cpu_address(uint16_t address)
{
	return
		(address == kIo)
		|| (address == kIe);
}

Mmu::Mmu(Component &cpu, Component &ppu, uint8_t *memory)
	: cpu(cpu)
	, ppu(ppu)
	, memory(memory)
{}

uint8_t Mmu::read8(uint16_t address)
{
	return memory[address];
}

uint16_t Mmu::read16(uint16_t address)
{
	return uint16_t(read8(address + 1) << 8) + read8(address);
}

void Mmu::write8(uint16_t addr, uint8_t value)
{
	if (addr == kIf)
	{
		value |= 0b1110'0000;
		cpu.write8(kIf, value);
		ppu.write8(kIf, value);
	}
	else if (addr == kIo) // P1/JOYP
	{
		value |= 0xCF;
	}
	else if (addr == 0xFF02) // Serial transfer control
	{
		value |= 0b0111'1100;
	}
	else if (is_ppu_address(addr))
	{
		if (addr == kDmaStartAddr)
		{
			for (int i = 0; i < 0xA0; i++)
				write8(kOam + i, read8((uint16_t(value) << 8) + i));
		}
		else
			ppu.write8(addr, value);
	}
	else if (is_cpu_address(addr))
	{
		cpu.write8(addr, value);
	}

	// Let the other components do their thing, then write regardless.
	memory[addr] = value;
}

void Mmu::write16(uint16_t address, uint16_t value)
{
	write8(address, static_cast<uint8_t>(value));
	write8(address + 1, (value & 0xFF00) >> 8);
}
