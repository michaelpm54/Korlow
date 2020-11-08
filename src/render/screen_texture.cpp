#include "render/screen_texture.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

constexpr int kLcdWidth{ 160 };
constexpr int kLcdHeight{ 144 };

void texture_init(GLuint* texture, int w, int h)
{
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void texture_free(GLuint* texture)
{
	glDeleteTextures(1, texture);
}

void texture_set_pixels(GLuint texture, const u8* pixels, int w, int h)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RED_INTEGER, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}
