#include <stdexcept>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "font.h"
#include "util.h"

static FT_Library ftLib;
int gW = 0;
int gH = 0;

void FT_Init()
{
	FT_Error err = FT_Init_FreeType(&ftLib);
	if (err)
	{
		throw std::runtime_error("FreeType init failed");
	}
}

void FT_Done()
{
	FT_Done_FreeType(ftLib);
}

Font::Font(const std::string &path)
{
	loadFontFace(path);
}

Font::~Font()
{
	glDeleteProgram(mProgram);
	glDeleteVertexArrays(1, &mVao);
	glDeleteBuffers(1, &mVbo);
	glDeleteTextures(1, &mTex);
	FT_Done_Face(*mFace);
}

void Font::loadFontFace(const std::string &path)
{
	if (mFace)
	{
		delete mFace;
	}
	mFace = new FT_Face();
	FT_Error err = FT_New_Face(ftLib, path.c_str(), 0, mFace);
	if (err != FT_Err_Ok)
	{
		if (err == FT_Err_Unknown_File_Format)
		{
			throw std::runtime_error("FT_New_Face failed: FT_Err_Unknown_File_Format");
		}
		else if (err == FT_Err_Cannot_Open_Resource)
		{
			throw std::runtime_error("FT_New_Face failed: FT_Err_Cannot_Open_Resource");	
		}
		else if (err == FT_Err_Invalid_File_Format)
		{
			throw std::runtime_error("FT_New_Face failed: FT_Err_Invalid_File_Format");	
		}
		else
		{
			throw std::runtime_error("FT_New_Face failed: Unhandled error code: " + std::to_string(err));	
		}		
	}

	FT_Set_Char_Size(*mFace, 0, 15*64, 0, 300);
	// FT_Set_Pixel_Sizes(*mFace, 0, 2);  

	buildAtlas();
}

void Font::buildAtlas()
{
	glGenVertexArrays(1, &mVao);
	glGenBuffers(1, &mVbo);
	glGenTextures(1, &mTex);
	mProgram = glCreateProgram();
	loadShaders(mProgram, "../assets/shaders/font.vs", "../assets/shaders/font.fs");

	glBindVertexArray(mVao);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (GLvoid *)(sizeof(GLfloat) * 2));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	for (int i = 32; i < 128; i++)
	{
		if (FT_Err_Ok != FT_Load_Char(*mFace, i, FT_LOAD_RENDER))
			continue;

		auto glyph = (*mFace)->glyph;

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, glyph->bitmap.width, glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		FontChar c;
		c.tex = tex;
		c.width = glyph->bitmap.width;
		c.height = glyph->bitmap.rows;
		c.advanceX = glyph->advance.x >> 6;
		c.advanceY = glyph->advance.y >> 6;
		c.bitmap_top = glyph->bitmap_top;
		c.bitmap_left = glyph->bitmap_left;
		mCharMap.push_back(c);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Font::renderText(const std::string &text, float x, float y)
{
	int size = text.size();
	if (!size)
	{
		return;
	}

	glUseProgram(mProgram);
	glBindVertexArray(mVao);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);

	glm::mat4 proj(glm::ortho(0.0f, 1280.0f, 1152.0f, 0.0f));

	for (int i = 0; i < size; i++)
	{
		FontChar &c = mCharMap[text[i] - 32];

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

		glBindTexture(GL_TEXTURE_2D, c.tex);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glm::mat4 mvp = proj;

		glUniformMatrix4fv(glGetUniformLocation(mProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += c.advanceX;
	}

	glBindVertexArray(0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
