#ifndef FONT_H
#define FONT_H

#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/gl.h>

void FT_Init();
void FT_Done();

class Font
{
public:
	Font(const std::string &path);
	~Font();
	void loadFontFace(const std::string &path);
	void buildAtlas();
	void renderText(const std::string &text, float x, float y);

private:
	FT_Face *mFace { nullptr };

	GLuint mVao { 0 };
	GLuint mVbo { 0 };
	GLuint mTex { 0 };
	GLuint mProgram { 0 };

	struct FontChar
	{
		GLuint tex;
		int width;
		int height;
		int advanceX;
		int advanceY;
		int bitmap_left;
		int bitmap_top;
	};

	std::vector<FontChar> mCharMap;
};

#endif // FONT_H
