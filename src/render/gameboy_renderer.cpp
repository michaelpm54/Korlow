#include "gameboy_renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl_util.h"

constexpr int kScreenScale = 1;
constexpr int kScreenSizeX = 160 * kScreenScale;
constexpr int kScreenSizeY = 144 * kScreenScale;

GameboyRenderer::GameboyRenderer()
	: mWidth(kScreenSizeX)
	, mHeight(kScreenSizeY)
	, proj_matrix(glm::ortho(0.0f, 160.0f, 0.0f, 144.0f))
{}

GameboyRenderer::~GameboyRenderer()
{
	destroyGLObjects();
}

void GameboyRenderer::initGL()
{
	createGLObjects();
}

void GameboyRenderer::render()
{
	if (!mEnabled)
		return;

	glUseProgram(mProgram);
	glBindVertexArray(mVao);

	glUniformMatrix4fv(glGetUniformLocation(mProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj_matrix));

	glm::mat4 mainModel(1.0f);
	mainModel = glm::scale(mainModel, glm::vec3(160.0f, 144.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(mProgram, "model"), 1, GL_FALSE, glm::value_ptr(mainModel));

	glBindTexture(GL_TEXTURE_2D, mFrameTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GameboyRenderer::update(const uint8_t* data)
{
	glBindTexture(GL_TEXTURE_2D, mFrameTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 160, 144, GL_RED_INTEGER, GL_UNSIGNED_BYTE, data);
}

void GameboyRenderer::createGLObjects()
{
	destroyGLObjects();

	mProgram = glCreateProgram();
	glGenTextures(1, &mFrameTexture);
	glGenVertexArrays(1, &mVao);
	glGenBuffers(1, &mVbo);

	glBindTexture(GL_TEXTURE_2D, mFrameTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 160, 144, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

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
}

void GameboyRenderer::destroyGLObjects()
{
	if (mProgram)
	{
		glDeleteProgram(mProgram);
	}
	if (mFrameTexture)
	{
		glDeleteTextures(1, &mFrameTexture);
	}
	if (mVao)
	{
		glDeleteVertexArrays(1, &mVao);
	}
	if (mVbo)
	{
		glDeleteBuffers(1, &mVbo);
	}
}
