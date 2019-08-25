#include <cmath>
#include <cstring>
#include <algorithm>
#include <iostream>
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

const int kWidth = 256;
const int kHeight = 256;
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

uint8_t paletteIndex(uint8_t byte0, uint8_t byte1, uint8_t pxIndex)
{
	return ((!!(byte1 & (0x80 >> pxIndex))) << 1) | (!!(byte0 & (0x80 >> pxIndex)));
}

void tileToPixels(uint8_t *palette, uint8_t *tile, uint8_t *pixels)
{
	int b = 0;
	for (int byte = 0; byte < 16; byte++)
	{
		for (int i = 0; i < 8; i++)
		{
			uint8_t mask = 0x80 >> i;
			uint8_t upper = !!(tile[byte] & mask);
			uint8_t lower = !!(tile[byte+1] & mask);
			uint8_t combined = (lower << 1) | upper;
			pixels[b++] = palette[combined];
		}
		byte++;
	}
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

	uint16_t bgMap = kBgMap0;
	if (lcdc & 0b0000'1000)
	{
		bgMap = kBgMap1;
	}

	for (int tiley = 0; tiley < 32; tiley++)
	{
		for (int tilex = 0; tilex < 32; tilex++)
		{
			uint8_t tile[16];
			uint8_t pixels[64];

			int patternIdx = mmu->mem[bgMap + (tiley * 32) + tilex];
			patternIdx = tileData == kTileRamSigned ? int8_t(patternIdx) : uint8_t(patternIdx);
			memcpy(tile, &mmu->mem[tileData + (patternIdx * 16)], 16);
			tileToPixels(mBgPalette, tile, pixels);

			int pxIdx = 0;
			for (int px_y = 0; px_y < 8; px_y++)
			{
				for (int px_x = 0; px_x < 8; px_x++)
				{
					int xoffs = tilex * 8;
					int abs_x = xoffs + px_x;
					int yoffs = tiley * 8;
					int abs_y = yoffs + px_y;
					mPixels[abs_y * kWidth + abs_x] = pixels[pxIdx++];
				}
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
	mBgPalette[1] = kShades[(val & 0xC) >> 2];
	mBgPalette[2] = kShades[(val & 0x30) >> 4];
	mBgPalette[3] = kShades[(val & 0xC0) >> 6];
}
