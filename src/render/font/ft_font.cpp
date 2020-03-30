#include "ft_font.h"

#include <iostream>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "render/gl_util.h"

FTFont::~FTFont()
{
	cleanup();
}

void FTFont::cleanup()
{
	glDeleteProgram(mProgram);
	for (auto& c : mCharMap)
	{
		glDeleteTextures(1, &c.tex);
	}
}

void FTFont::load(const std::string& path, int size)
{
	if (path.empty())
		return;

	try {
		mFace = FTUtil::LoadFace(path);
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
	}

	setSize(size);
}

void FTFont::setSize(int size)
{
	FTUtil::SetCharSize(mFace, size);
	rebuildAtlas();
}

void FTFont::rebuildAtlas()
{
	cleanup();

	mProgram = glCreateProgram();
	loadShaders(mProgram, "assets/shaders/font.vs", "assets/shaders/font.fs");

	for (int i = 32; i < 128; i++)
	{
		try {
			FTUtil::LoadChar(mFace, i, FT_LOAD_RENDER);
		}
		catch (...) {
			printf("Failed to load char '%c'\n", i);
			continue;
		}

		auto glyph { mFace->glyph };

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, glyph->bitmap.width, glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		FTChar c;
		c.tex = tex;
		c.width = glyph->bitmap.width;
		c.height = glyph->bitmap.rows;
		c.advanceX = glyph->advance.x >> 6;
		c.advanceY = glyph->advance.y >> 6;
		c.bitmap_top = glyph->bitmap_top;
		c.bitmap_left = glyph->bitmap_left;
		mCharMap.push_back(c);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void FTFont::drawString(const RenderedString& rs) const
{
	glUseProgram(mProgram);
	glBindVertexArray(rs.vao);
	glBindBuffer(GL_ARRAY_BUFFER, rs.vbo);

	glm::mat4 proj(glm::ortho(0.0f, 1280.0f, 1152.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(mProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(proj));

	for (int i = 0; i < rs.size; i++)
	{
		glBindTexture(GL_TEXTURE_2D, rs.textures[i]);
		glDrawArrays(GL_TRIANGLES, i * 6, 6);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

RenderedString FTFont::createString(const std::string& text, float x, float y) const
{
	auto size { text.size() };
	if (!size)
		return RenderedString{ .size = 0, .vbo = 0 };

	RenderedString rs;
	rs.size = size;
	rs.textures.resize(size);

	glGenBuffers(1, &rs.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rs.vbo);
	glBufferData(GL_ARRAY_BUFFER, size * 6 * 4 * sizeof(GLfloat), nullptr, GL_STATIC_DRAW);
	glObjectLabel(GL_BUFFER, rs.vbo, text.size(), text.c_str());

	glGenVertexArrays(1, &rs.vao);
	glBindVertexArray(rs.vao);
	glBindBuffer(GL_ARRAY_BUFFER, rs.vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (GLvoid *)(sizeof(GLfloat) * 2));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	GLintptr bufferOffset { 0 };

	for (int i = 0; i < size; i++)
	{
		const unsigned int charCode = text[i] - 32;

		if (charCode >= mCharMap.size())
			continue;
			
		const FTChar &c { mCharMap[charCode] };

		float cx = x + c.bitmap_left;
		float cy = y - c.bitmap_top;

		GLfloat vertices[] =
		{
			cx,           cy,            0.0f, 0.0f,
			cx + c.width, cy,            1.0f, 0.0f,
			cx + c.width, cy + c.height, 1.0f, 1.0f,
			cx,           cy,            0.0f, 0.0f,
			cx + c.width, cy + c.height, 1.0f, 1.0f,
			cx,           cy + c.height, 0.0f, 1.0f,
		};

		rs.textures[i] = c.tex;

		glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, sizeof(vertices), vertices);
		bufferOffset += 6 * 4 * sizeof(GLfloat);

		x += c.advanceX;
	}

	drawString(rs);

	return rs;
}
