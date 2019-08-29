#ifndef GPU_H
#define GPU_H

#include <cstdint>
#include <GL/gl.h>

class MMU;

class GPU
{
public:
	GPU();
	~GPU();
	void initOpenGL();
	void frame();
	void updatePixels(int line);
	void reset();
	void createGLObjects();
	void destroyGLObjects();
	void tick(int cycles);
	void setBgPalette(uint8_t val);

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
};

#endif // GPU_H
