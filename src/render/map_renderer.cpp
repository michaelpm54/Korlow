#include "map_renderer.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_util.h"

constexpr int kScreenScale = 1;
constexpr float kTextureSizeX = 32 * kScreenScale;
constexpr float kTextureSizeY = 32 * 2 * kScreenScale;

MapRenderer::MapRenderer()
	: proj_matrix(glm::ortho(0.0f, 32.0f, 0.0f, 64.0f))
{
	model_matrix = glm::scale(glm::vec3(kTextureSizeX, kTextureSizeY, 1.0f));
}

MapRenderer::~MapRenderer()
{
	destroyGLObjects();
}

void MapRenderer::initGL()
{
	createGLObjects();
}

void MapRenderer::render()
{
	if (!mEnabled)
		return;

	glUseProgram(mProgram);
	glBindVertexArray(mVao);

	glUniformMatrix4fv(glGetUniformLocation(mProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj_matrix));
	glUniformMatrix4fv(glGetUniformLocation(mProgram, "model"), 1, GL_FALSE, glm::value_ptr(model_matrix));

	glBindTexture(GL_TEXTURE_2D, mMapTex);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MapRenderer::update(const uint8_t* data)
{
	glBindTexture(GL_TEXTURE_2D, mMapTex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 32, 64, GL_RED_INTEGER, GL_UNSIGNED_BYTE, data);
}

void MapRenderer::createGLObjects()
{
	destroyGLObjects();

	mProgram = glCreateProgram();
	glGenVertexArrays(1, &mVao);
	glGenBuffers(1, &mVbo);

	loadShaders(mProgram, "assets/shaders/ssquad.vs", "assets/shaders/ssquad.fs");

	GLfloat vertices[] =
	{
		/* xyzw */ 0.0f, 0.0f, 0.0f, 1.0f, /* uv */ 0.0f, 1.0f,
		/* xyzw */ 1.0f, 0.0f, 0.0f, 1.0f, /* uv */ 1.0f, 1.0f,
		/* xyzw */ 1.0f, 1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 0.0f,
		/* xyzw */ 0.0f, 0.0f, 0.0f, 1.0f, /* uv */ 0.0f, 1.0f,
		/* xyzw */ 1.0f, 1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 0.0f,
		/* xyzw */ 0.0f, 1.0f, 0.0f, 1.0f, /* uv */ 0.0f, 0.0f,
	};

	glBindVertexArray(mVao);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 6, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid *)(sizeof(GLfloat) * 0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid *)(sizeof(GLfloat) * 4));
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	/*
	Map: a 32x32 array of tile indices, which are 1 byte long.
	a tile index refers to a location in the tile data starting at either
	0x8000 or 0x9000 depending on whether LCDC.4 is set.
	0 = 0x8000 and the tile index is unsigned
	1 = 0x9000 and the tile index is signed
	Each tile data set is 0x1000 bytes long.

	For the map view I will create a texture showing an RGB colour from 0-255 based on the index,
	for each index.

	There are two maps of size 0x400. One is at 0x9800 and the other is at 0x9C00.
	Which one is used is set based on LCDC.3.
	*/
	glGenTextures(1, &mMapTex);

	glBindTexture(GL_TEXTURE_2D, mMapTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 32, 64, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MapRenderer::destroyGLObjects()
{
	if (mProgram)
	{
		glDeleteProgram(mProgram);
	}
	if (mVao)
	{
		glDeleteVertexArrays(1, &mVao);
	}
	if (mVbo)
	{
		glDeleteBuffers(1, &mVbo);
	}
	if (mMapTex)
	{
		glDeleteTextures(1, &mMapTex);
	}
}

