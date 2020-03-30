#ifndef FT_FONT_H
#define FT_FONT_H

#include <string>
#include <vector>

#include "render/font/ft_util.h"
#include "render/opengl.h"

class FTFont;

struct RenderedString
{
	FTFont *font;
	int size;
	GLuint vbo;
	GLuint vao;
	std::vector<GLuint> textures;
};

class FTFont
{
public:
	~FTFont();

	void cleanup();
	void load(const std::string &path, int size = 12);
	void setSize(int size);
	void drawString(const RenderedString &rs) const;
	RenderedString createString(const std::string& text, float x, float y) const;

private:
	void rebuildAtlas();

private:
	struct FTChar
	{
		GLuint tex;
		int width;
		int height;
		int advanceX;
		int advanceY;
		int bitmap_left;
		int bitmap_top;
	};

private:
	FT_Face mFace { nullptr };
	GLuint mProgram { 0 };
	std::vector<FTChar> mCharMap;
};

#endif // FT_FONT_H
