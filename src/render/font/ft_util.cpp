#include "ft_util.h"

#include <stdexcept>

namespace FTUtil
{

static FT_Library ftLib;

void Init()
{
	FT_Error err = FT_Init_FreeType(&ftLib);
	if (err)
	{
		throw std::runtime_error("FreeType init failed");
	}
}

void Done()
{
	FT_Done_FreeType(ftLib);
}

FT_Face LoadFace(const std::string& path)
{
	FT_Face face;
	FT_Error err = FT_New_Face(ftLib, path.c_str(), 0, &face);
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
	return face;
}

void LoadChar(FT_Face face, FT_ULong index, FT_Int32 flags)
{
	if (FT_Err_Ok != FT_Load_Char(face, index, flags))
		throw std::runtime_error("Failed to load char");
}

void SetCharSize(FT_Face face, int size)
{
	FT_Set_Char_Size(face, 0, size*64, 0, 300);
}

}
