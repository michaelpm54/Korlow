#ifndef GAMEBOY_RENDERER_H
#define GAMEBOY_RENDERER_H

#include <glm/glm.hpp>

#include "opengl.h"
#include "renderer.h"

class GameboyRenderer : public Renderer
{
public:
	GameboyRenderer();
	~GameboyRenderer();

	void initGL() override;
	void render() override;

	void update(const uint8_t *pixels);

private:
	void createGLObjects();
	void destroyGLObjects();

private:
	GLuint mProgram { 0 };
	GLuint mFrameTexture { 0 };
	GLuint mVao { 0 };
	GLuint mVbo { 0 };

	int mWidth { 0 };
	int mHeight { 0 };
	int mNumPixels { 0 };

	glm::mat4 proj_matrix { 1.0f };
};

#endif // GAMEBOY_RENDERER_H

