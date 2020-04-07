#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <vector>

#include "cpu/cpu.h"
#include "mmu.h"
#include "ppu.h"

struct Gameboy
{
	Gameboy();

	void set_components(Cpu &cpu, Ppu &ppu, Mmu &mmu);

	uint8_t *get_memory();
	bool is_running() const;
	void set_rom(const std::vector<uint8_t> &bytes);
	void tick();
	void reset();
	const uint8_t *get_lcd_pixels() const;

private:
	std::vector<uint8_t> memory;
	Cpu *cpu { nullptr };
	Ppu *ppu { nullptr };
	Mmu *mmu { nullptr };
	bool quit { false };
};

#endif // GAMEBOY_H
