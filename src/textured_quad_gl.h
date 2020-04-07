#ifndef TEXTURED_QUAD_GL_H
#define TEXTURED_QUAD_GL_H

#include "render/opengl.h"

#include <glm/glm.hpp>

class TexturedQuadGL
{
public:
	TexturedQuadGL() = default;
	~TexturedQuadGL();

	void create(int w = 0, int h = 0);
	void destroy();
	void set_size(int w, int h);
	void set_pixels(const uint8_t *pixels);
	void draw(GLuint program, const glm::mat4 &projection);
	void set_enabled(bool);

private:
	int w { 0 };
	int h { 0 };
	bool exists { false };
	bool enabled { true };
	GLuint texture;
	GLuint vao;
	GLuint vbo;
};

#endif // TEXTURED_QUAD_GL_H
