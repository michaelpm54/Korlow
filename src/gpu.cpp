#include <cmath>
#include <algorithm>
#include "gpu.h"
#include "mmu.h"
#include "memory_map.h"

constexpr uint8_t kShades[4] =
{
	0x00,
	0x3F,
	0x7E,
	0xFF
};

const int kWidth = 16*8;
const int kHeight = 12*8;
const int kComponents = 1;
const int kNumPixels = kWidth * kHeight * kComponents;

GPU::GPU()
	:
	mPixels(new uint8_t[kNumPixels])
{
	memcpy(mBgPalette, kShades, 4);
}

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
	uint8_t lcdc = mmu->mem[kLcdc];

	if (!(lcdc & 0x80))
		return;

	uint16_t tileData = kTileRamSigned;
	if (lcdc & 0b0001'0000)
	{
		tileData = kTileRamUnsigned;
	}

	uint16_t bgMap = kBgMapUnsigned;
	if (lcdc & 0b0000'1000)
	{
		bgMap = kBgMapSigned;
	}

	/*
		192 tiles

		16 bytes per tile
		2 bytes per line
		8 lines
	*/

	tileData = 0xC000;

	int i = 0;
	for (int tile = 0; tile < 192; tile++)
	{
		int tileOffset = tile * 16;
		for (int line = 0; line < 8; line++)
		{
			int lineOffset = tileOffset + (line * 2);
			uint8_t d0 = mmu->mem[tileData + lineOffset];
			uint8_t d1 = mmu->mem[tileData + lineOffset + 1];
			for (int px = 0; px < 8; px++)
			{
				uint8_t c = 0;
				c |= !!(d0 & (0x80 >> px));
				c |= (!!(d1 & (0x80 >> px))) << 1;
				mPixels[i++] = mBgPalette[c];
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

void GPU::tick(int cycles)
{
}

void GPU::setBgPalette(uint8_t val)
{
	mBgPalette[0] = kShades[val & 0x3];
	mBgPalette[1] = kShades[val & 0xC];
	mBgPalette[2] = kShades[val & 0x30];
	mBgPalette[3] = kShades[val & 0xC0];
}
