#ifndef FT_UTIL_H
#define FT_UTIL_H

#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace FTUtil
{

void Init();
void Done();

FT_Face LoadFace(const std::string &path);

void LoadChar(FT_Face face, FT_ULong index, FT_Int32 flags);
void SetCharSize(FT_Face face, int size);

}

#endif // FT_UTIL_H
