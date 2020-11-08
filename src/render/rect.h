#ifndef KORLOW_RECT_H
#define KORLOW_RECT_H

#include <glad/glad.h>

#include <glm/mat4x4.hpp>

struct Rect {
	GLuint vao{GL_NONE};
	GLuint vbo{GL_NONE};
};

void rect_init(Rect *rect);
void rect_free(Rect *rect);
void rect_draw(Rect *rect, GLuint texture, GLuint program, const glm::mat4 &projection, const glm::mat4 &scale);

#endif // KORLOW_RECT_H
