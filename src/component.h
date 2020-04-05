#ifndef COMPONENT_H
#define COMPONENT_H

struct Component
{
	Component() {}
	Component(const Component &) = delete;
	virtual ~Component() {}

	virtual void reset() = 0;

	virtual uint8_t read8(uint16_t address) { return 0xFF; }
	virtual uint16_t read16(uint16_t address) { return 0xFFFF; }
	virtual void write8(uint16_t address, uint8_t value) {}
	virtual void write16(uint16_t address, uint16_t value) {}
};

#endif // COMPONENT_H
