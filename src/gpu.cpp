#include <cmath>
#include <algorithm>
#include "gpu.h"

GPU::GPU()
	:
	mPixels(new uint8_t[160*144])
{}

GPU::~GPU()
{
	destroyGLObjects();
	delete mPixels;
}

void GPU::reset()
{
	createGLObjects();
	std::fill_n(mPixels, 160*144, 0);
}

void GPU::destroyGLObjects()
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

void GPU::createGLObjects()
{
	destroyGLObjects();

	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);

	mProgram = glCreateProgram();
	glGenTextures(1, &mFrameTexture);
	glGenVertexArrays(1, &mVao);
	glGenBuffers(1, &mVbo);

	glBindTexture(GL_TEXTURE_2D, mFrameTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 160, 144, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	const char *vsSrc = "\
		#version 460 core\n\
		layout(location=0) in vec4 aPos;\
	 	layout(location=1) in vec2 aTexCoord;\
	 	out vec2 texCoord;\
	 	void main()\
	 	{\
		 	gl_Position=aPos;\
		 	texCoord = aTexCoord;\
		}";
	const char *fsSrc = "\
		#version 460 core\n\
	 	in vec2 texCoord;\
	 	out vec4 colour;\
	 	uniform sampler2D tex;\
	 	void main()\
	 	{\
	 		colour = texture(tex, texCoord);\
		}";
	glShaderSource(vs, 1, &vsSrc, nullptr);
	glShaderSource(fs, 1, &fsSrc, nullptr);
	glCompileShader(vs);
	glCompileShader(fs);
	glAttachShader(mProgram, vs);
	glAttachShader(mProgram, fs);
	glLinkProgram(mProgram);
	glDetachShader(mProgram, vs);
	glDetachShader(mProgram, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	GLfloat vertices[] =
	{
		/* xyzw */ -1.0f, -1.0f, 0.0f, 1.0f, /* uv */ 0.0f, 1.0f,
		/* xyzw */ 1.0f, -1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 1.0f,
		/* xyzw */ 1.0f, 1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 0.0f,
		/* xyzw */ -1.0f, -1.0f, 0.0f, 1.0f, /* uv */ 0.0f, 1.0f,
		/* xyzw */ 1.0f, 1.0f, 0.0f, 1.0f, /* uv */ 1.0f, 0.0f,
		/* xyzw */ -1.0f, 1.0f, 0.0f, 1.0f, /* uv */ 0.0f, 0.0f,
	};

	glBindVertexArray(mVao);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 6, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid *)(sizeof(GLfloat) * 0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid *)(sizeof(GLfloat) * 4));
	glBindVertexArray(0);
}

void GPU::updatePixels()
{
	static int x = 0;
	static int y = 0;

	int idx = y*160+x;
	mPixels[idx] = 0xFF;

	if (x == 160)
	{
		x = 0;
		y++;
	}
	if (y == 144)
	{
		y = 0;
	}
	x++;

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 160, 144, GL_RED, GL_UNSIGNED_BYTE, mPixels);
}

void GPU::frame()
{
	updatePixels();
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(mProgram);
	glBindVertexArray(mVao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glUseProgram(0);
	glBindVertexArray(0);
}
