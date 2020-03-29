#ifndef GPU_H
#define GPU_H

#include <cstdint>
#include <array>

class OpenGLWidget;

struct GpuRegisters
{
	uint8_t &if_;
	uint8_t &lcdc;
	uint8_t &stat;
	uint8_t &scy;
	uint8_t &scx;
	uint8_t &ly;
	uint8_t &lyc;
	uint8_t &dmaStartAddr;
	uint8_t &bgPalette;
	uint8_t &obj0Palette;
	uint8_t &obj1Palette;
	uint8_t &windowY;
	uint8_t &windowX;
};

struct GpuMem
{
	uint8_t *oam;
	uint8_t *map0;
	uint8_t *map1;
	uint8_t *tilesSigned;
	uint8_t *tilesUnsigned;
};

struct sprite_t
{
	uint8_t y;
	uint8_t x;
	uint8_t patternNum;
	uint8_t flags;
};

class GPU
{
public:
	GPU(GpuRegisters, GpuMem, OpenGLWidget*);
	~GPU();
	void drawScanline(int line);
	void reset();
	void tick(int cycles);
	void setBgPalette(uint8_t val);

	const uint8_t* getMap();
	const uint8_t* getPixels();
	void updateMap();

	void setSpritePalette(int paletteIdx, uint8_t val);

private:
	void setPixel(int x, int y, uint8_t colour);
	void drawSprite(const sprite_t &sprite);

public:
	int mode { MODE_OAM };

private:
	enum Mode
	{
		MODE_HBLANK = 0,
		MODE_VBLANK = 1,
		MODE_OAM = 2,
		MODE_OAM_VRAM = 3,
	};

	uint8_t *mPixels { nullptr };
	uint8_t mBgPalette[4];
	int mModeClock { 0 };
	bool mSpritesChanged { false };
	std::array<sprite_t, 40> mSprites;
	uint8_t *mMapData { nullptr };
	uint8_t mSpritePalettes[2][4];

	GpuRegisters reg;
	GpuMem mem;
};

#endif // GPU_H
