#ifndef GPU_H
#define GPU_H

#include <epoxy/gl.h>

class MMU;

class GPU
{
public:
	GPU();
	~GPU();
	void frame();
	void updatePixels();
	void reset();
	void createGLObjects();
	void destroyGLObjects();

	void tick(int cycles);

	void setBgPalette(uint8_t val);

public:
	MMU *mmu { nullptr };

private:
	GLuint mProgram { 0 };
	GLuint mFrameTexture { 0 };
	GLuint mVao { 0 };
	GLuint mVbo { 0 };
	uint8_t *mPixels { nullptr };
	uint8_t mBgPalette[4];
};

#endif // GPU_H
