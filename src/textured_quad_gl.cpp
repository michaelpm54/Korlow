#include "textured_quad_gl.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

TexturedQuadGL::~TexturedQuadGL()
{
	destroy();
}

void TexturedQuadGL::create(int w, int h)
{
	if (exists)
		destroy();

	this->w = w;
	this->h = h;

	glGenTextures(1, &texture);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLfloat vertices[] =
	{
		/* xyzw */ 0.0f, 0.0f, 0.0f, 1.0f, /* uv */ 0.0f, 1.0f,
		/* xyzw */ 1.0f, 0.0f, 0.0f, 1.0f, /* uv */ 1.0f, 1.0f,
		/* xyzw */ 1.0f, 1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 0.0f,
		/* xyzw */ 0.0f, 0.0f, 0.0f, 1.0f, /* uv */ 0.0f, 1.0f,
		/* xyzw */ 1.0f, 1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 0.0f,
		/* xyzw */ 0.0f, 1.0f, 0.0f, 1.0f, /* uv */ 0.0f, 0.0f,
	};

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 6, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid *)(sizeof(GLfloat) * 0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid *)(sizeof(GLfloat) * 4));
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	exists = true;
}

void TexturedQuadGL::destroy()
{
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	exists = false;
}

void TexturedQuadGL::set_size(int w, int h)
{
	this->w = w;
	this->h = h;
	create(w, h);
}

void TexturedQuadGL::set_pixels(const uint8_t* pixels)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RED_INTEGER, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TexturedQuadGL::draw(GLuint program, const glm::mat4 &projection)
{
	if (!enabled)
	{
		return;
	}

	glUseProgram(program);
	glBindVertexArray(vao);

	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	auto upscale { glm::scale(glm::vec3{w, h, 1.0f}) };
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(upscale));

	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}

void TexturedQuadGL::set_enabled(bool value)
{
	enabled = value;
}
