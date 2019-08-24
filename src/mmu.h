#ifndef MMU_H
#define MMU_H

#include <cstdint>
#include <string>

class GPU;

class MMU
{
public:
	MMU() = default;
	~MMU();

	void reset(uint8_t *biosData = nullptr);
	void setBios(uint8_t *biosData);
	void setRom(uint8_t *romData, int romSize);

	uint8_t read8(uint16_t addr);
	uint16_t read16(uint16_t addr);

	void write8(uint16_t addr, uint8_t val);
	void write16(uint16_t addr, uint16_t val);

	void ioWrite8(uint16_t addr, uint8_t val);

	void switchToRom();

private:
	uint8_t *mMem { nullptr };
	uint8_t *mBios { nullptr };
	bool mInBios { true };
	std::string mSerialData;

	friend class GPU;
};

#endif // MMU_H
