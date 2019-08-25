#ifndef MMU_H
#define MMU_H

#include <cstdint>
#include <memory>
#include <string>

struct MMU
{
// data
	std::shared_ptr<uint8_t[]> mem { nullptr };
	uint8_t *bios { nullptr };
	bool inBios { true };
	std::string serialData;

// functions
	void init();

	uint8_t read8(uint16_t addr);
	uint16_t read16(uint16_t addr);
	void write8(uint16_t addr, uint8_t val);
	void write16(uint16_t addr, uint16_t val);
	void or8(uint16_t addr, uint8_t val);
	void or16(uint16_t addr, uint16_t val);
	void and8(uint16_t addr, uint8_t val);
	void and16(uint16_t addr, uint16_t val);

private:
	void io_write8(uint16_t addr, uint8_t val);
};

#endif // MMU_H
