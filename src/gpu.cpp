#include <cmath>
#include <algorithm>
#include "gpu.h"
#include "mmu.h"

const int kWidth = 16*8;
const int kHeight = 12*8;
const int kComponents = 1;
const int kNumPixels = kWidth * kHeight * kComponents;

GPU::GPU()
	:
	mPixels(new uint8_t[kNumPixels])
{}

GPU::~GPU()
{
	destroyGLObjects();
	delete mPixels;
}

void GPU::reset()
{
	std::fill_n(mPixels, kNumPixels, 0x00);
	createGLObjects();
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, kWidth, kHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, mPixels);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	const char *vsSrc = "\
		#version 460 core\n\
		layout(location=0) in vec4 aPos;\
	 	layout(location=1) in vec2 aTexCoord;\
	 	out vec2 texCoord;\
	 	void main()\
	 	{\
		 	gl_Position = aPos;\
		 	texCoord = aTexCoord;\
		}";
	const char *fsSrc = "\
		#version 460 core\n\
	 	in vec2 texCoord;\n\
	 	out vec4 colour;\n\
	 	uniform usampler2D tex;\n\
	 	void main()\n\
	 	{\n\
	 		uint c = texture(tex, texCoord).r;\n\
	 		colour = vec4(0, float(c)/255.0f, 0, 1);\n\
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
	uint8_t lcdc = mmu->mMem[0xFF40];

	if (!(lcdc & 0x80))
		return;

	bool signedIdx = true;

	uint8_t *bgData = &mmu->mMem[0x9000]; // 0x8800
	if (lcdc & 0b0001'0000)
	{
		bgData = &mmu->mMem[0x8000];
		signedIdx = false;
	}

	uint8_t *bgMap = &mmu->mMem[0x9800];
	if (lcdc & 0b0000'1000)
		bgMap = &mmu->mMem[0x9C00];

	static constexpr uint8_t palette[4] =
	{
		0x15,
		0x75,
		0xB0,
		0xF5
	};

	/*
		192 tiles

		16 bytes per tile
		2 bytes per line
		8 lines
	*/

	int i = 0;
	for (int tile = 0; tile < 192; tile++)
	{
		int tileOffset = tile * 16;
		for (int line = 0; line < 8; line++)
		{
			int lineOffset = tileOffset + (line * 2);
			for (int px = 0; px < 8; px++)
			{
				uint8_t c = 0;
				uint8_t lo = bgData[lineOffset];
				uint8_t hi = bgData[lineOffset+1];
				c |= !!(lo & (0x80 >> px));
				c |= (!!(hi & (0x80 >> px))) << 1;
				mPixels[i++] = palette[c];
			}
		}
	}

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, kWidth, kHeight, GL_RED_INTEGER, GL_UNSIGNED_BYTE, mPixels);

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
