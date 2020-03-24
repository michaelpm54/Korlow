#ifndef MMU_H
#define MMU_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "types.h"

class GPU;

struct MMU
{
	MMU();

	void init(GPU *gpu);

    // data
	GPU *mGpu { nullptr };
	std::vector<std::uint8_t> mem;
	uint8_t *bios { nullptr };
	bool inBios { true };
	std::string serialData;

	// functions
	void setRom(const std::vector<std::uint8_t> &bytes);

	uint8_t read8(uint16_t addr);
	uint16_t read16(uint16_t addr);
	void write8(uint16_t addr, uint8_t val);
	void write16(uint16_t addr, uint16_t val);
	void or8(uint16_t addr, uint8_t val);
	void or16(uint16_t addr, uint16_t val);
	void and8(uint16_t addr, uint8_t val);
	void and16(uint16_t addr, uint16_t val);

private:
	void oam_write8(uint16_t addr, uint8_t val);
	void io_write8(uint16_t addr, uint8_t val);

	bool mInBios { false };
};

#endif // MMU_H
