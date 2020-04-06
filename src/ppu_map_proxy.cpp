#include "ppu_map_proxy.h"

#include "memory_map.h"
#include "ppu.h"

/*
int getMapIndex(int x, int y)
{
return ((y / 8) * 32) + (x / 8);
}

int getMapEntry(uint8_t *map, int x, int y)
{
return map[getMapIndex(x, y)];
}
*/

/*
void GPU::updateMap()
{
	if (!(reg.lcdc & (1U << 3)))
	{
		for (int i = 0; i < 0x400; i++)
		{
			mMapData[i] = mem.map0[i];
		}

		int left = getMapIndex(reg.scx, reg.scy);
		int right = left + 20;
		int bottomLeft = (left + (18 * 32)) % 0x400;
		int bottomRight = (right + (18 * 32)) % 0x400;

		for (int i = 0; i <= 18; i++)
		{
			int offsetLeft = (left + (i * 32)) % 0x400;
			int offsetRight = (right + (i * 32)) % 0x400;
			mMapData[offsetLeft] = 0xFF;
			mMapData[offsetRight] = 0xFF;
		}

		memset(&mMapData[left], 0xFF, 20);
		memset(&mMapData[bottomLeft], 0xFF, 20);
	}
	else
	{
		for (int i = 0; i < 0x400; i++)
		{
			mMapData[0x400 + i] = mem.map1[i];
		}

		int left = getMapIndex(reg.scx, reg.scy);
		int right = left + 20;
		int bottomLeft = left + (18 * 32);

		memset(&mMapData[left], 0xFF, 20);
		memset(&mMapData[bottomLeft], 0xFF, 20);
		for (int i = 0; i <= 18; i++)
		{
			mMapData[left + (i * 32)] = 0xFF;
			mMapData[right + (i * 32)] = 0xFF;
		}
	}
}
*/

PpuMapProxy::PpuMapProxy(Ppu &ppu)
	: ppu(ppu)
	, map_pixels(0x800)
{}

void PpuMapProxy::reset()
{
}

void PpuMapProxy::write8(uint16_t address, uint8_t value)
{
	if (address >= kMap0 && address <= kMap1)
	{
		uint16_t map_address = address - kMap0;
		int x = map_address % 32;
		int y = map_address / 32;
		int px_index { y * 32 + x };
		map_pixels[px_index] = value;
	}
	else if (address == kScx)
	{
		/*int startx = value / 8;
		int starty = ppu.registers.scy / 8;
		for (int i = 0; i < 20; i++)
		{
			int idx { (starty * 32) + startx + i };
			map_pixels[idx] = ppu.map0[idx];
			map_pixels[idx] = 0xFF;
		}
		for (int i = 0; i < 18; i++)
		{
			int idx { ((starty + i) * 32) + startx };
			map_pixels[idx] = ppu.map0[idx];
			map_pixels[idx] = 0xFF;
		}
		starty += 18;
		for (int i = 0; i < 20; i++)
		{
			int idx { (starty * 32) + startx + i };
			map_pixels[idx] = ppu.map0[idx];
			map_pixels[idx] = 0xFF;
		}
		starty -= 18;
		startx += 20;
		for (int i = 0; i <= 18; i++)
		{
			int idx { ((starty + i) * 32) + startx };
			map_pixels[idx] = ppu.map0[idx];
			map_pixels[idx] = 0xFF;
		}*/
	}
	else if (address == kScy)
	{
		/*int startx = ppu.registers.scx / 8;
		int starty = value / 8;
		for (int i = 0; i < 20; i++)
		{
			int idx { (starty * 32) + startx + i };
			map_pixels[idx] = ppu.map0[idx];
			map_pixels[idx] = 0xFF;
		}
		for (int i = 0; i < 18; i++)
		{
			int idx { ((starty + i) * 32) + startx };
			map_pixels[idx] = ppu.map0[idx];
			map_pixels[idx] = 0xFF;
		}
		starty += 18;
		for (int i = 0; i < 20; i++)
		{
			int idx { (starty * 32) + startx + i };
			map_pixels[idx] = ppu.map0[idx];
			map_pixels[idx] = 0xFF;
		}
		starty -= 18;
		startx += 20;
		for (int i = 0; i <= 18; i++)
		{
			int idx { ((starty + i) * 32) + startx };
			map_pixels[idx] = ppu.map0[idx];
			map_pixels[idx] = 0xFF;
		}*/
	}

	ppu.write8(address, value);
}

const uint8_t *PpuMapProxy::get_pixels() const
{
	return map_pixels.data();
}
