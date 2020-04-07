#include "gameboy.h"

#include "memory_map.h"

#include "constants.h"

Gameboy::Gameboy()
	: memory(0x10000)
{}

bool Gameboy::is_running() const
{
	return !quit;
}

void Gameboy::set_rom(const std::vector<uint8_t>& bytes)
{
	std::copy_n(bytes.data(), std::min(0xFFFF, static_cast<int>(bytes.size())), &memory[0]);
}

void Gameboy::tick()
{
	int cycles = 0;
	while (cycles < kMaxCyclesPerFrame && cpu->is_enabled())
	{
		cycles += cpu->tick(*mmu);
		ppu->tick(cycles);
	}
}

void Gameboy::reset()
{
	cpu->reset();
	ppu->reset();
	mmu->reset();

	// Sound
	mmu->write8(kNr10, 0x80);
	mmu->write8(kNr11, 0xBF);
	mmu->write8(kNr12, 0xF3);
	mmu->write8(kNr14, 0xBF);
	mmu->write8(kNr21, 0x3F);
	mmu->write8(kNr24, 0xBF);
	mmu->write8(kNr30, 0x7F);
	mmu->write8(kNr31, 0xFF);
	mmu->write8(kNr32, 0x9F);
	mmu->write8(kNr34, 0xBF);
	mmu->write8(kNr41, 0xFF);
	mmu->write8(kNr44, 0xBF);
	mmu->write8(kNr50, 0x77);
	mmu->write8(kNr51, 0xF3);
	mmu->write8(kNr52, 0xF1);

	// CPU registers
	mmu->write8(kIo, 0xCF);
	mmu->write8(kIf, 0xE1);
	mmu->write8(kIe, 0x00);

	// PPU registers
	mmu->write8(kLcdc, 0x91);
	mmu->write8(kStat, 0x00);
	mmu->write8(kScy, 0x00);
	mmu->write8(kScy, 0x00);
	mmu->write8(kLy, 0x00);
	mmu->write8(kLyc, 0x00);
	mmu->write8(kBgPalette, 0xFC);
	mmu->write8(kObj0Palette, 0xFF);
	mmu->write8(kObj1Palette, 0xFF);
	mmu->write8(kWy, 0x00);
	mmu->write8(kWx, 0x00);
}

uint8_t *Gameboy::get_memory()
{
	return memory.data();
}

void Gameboy::set_components(Cpu& cpu, Ppu& ppu, Mmu& mmu)
{
	this->cpu = &cpu;
	this->ppu = &ppu;
	this->mmu = &mmu;
}

const uint8_t* Gameboy::get_lcd_pixels() const
{
	return ppu->get_pixels();
}
