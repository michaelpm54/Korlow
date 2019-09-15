#ifndef GPU_H
#define GPU_H

#include <cstdint>
#include <array>
#include <GL/gl.h>
#include <glm/glm.hpp>

struct sprite_t
{
	uint8_t y;
	uint8_t x;
	uint8_t patternNum;
	uint8_t flags;
};

class MMU;

class GPU
{
public:
	GPU();
	~GPU();
	void initOpenGL();
	void frame();
	void drawScanline(int line);
	void reset();
	void createGLObjects();
	void destroyGLObjects();
	void tick(int cycles);
	void setBgPalette(uint8_t val);

	void setSize(int w, int h);

	// Map
	void updateMap();

	void setSpritePalette(int paletteIdx, uint8_t val);

private:
	void setPixel(int x, int y, uint8_t colour);
	void drawSprite(const sprite_t &sprite);

public:
	MMU *mmu { nullptr };
	int mode { MODE_OAM };

private:
	enum Mode
	{
		MODE_HBLANK = 0,
		MODE_VBLANK = 1,
		MODE_OAM = 2,
		MODE_OAM_VRAM = 3,
	};

	GLuint mProgram { 0 };
	GLuint mFrameTexture { 0 };
	GLuint mVao { 0 };
	GLuint mVbo { 0 };
	uint8_t *mPixels { nullptr };
	uint8_t mBgPalette[4];
	int mModeClock { 0 };
	bool mSpritesChanged { false };
	std::array<sprite_t, 40> mSprites;

	int mWidth { 0 };
	int mHeight { 0 };
	int mNumPixels { 0 };

	glm::mat4 mProjMatrix { 1.0f };
	glm::mat4 mMainModel { 1.0f };
	glm::mat4 mMapModel { 1.0f };

	// Map
	GLuint mMapTex;
	GLuint mMapBuf;
	GLuint mMapVao;
	uint8_t *mMapData { nullptr };

	uint8_t mSpritePalettes[2][4];
};

#endif // GPU_H
