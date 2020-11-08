#include "render/gl_texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

constexpr int kLcdWidth {160};
constexpr int kLcdHeight {144};

void texture_init(Texture* texture, int w, int h, int num_components)
{
    assert(num_components == 1 || num_components == 4);

    texture->w = w;
    texture->h = h;
    GLint internal_format;

    switch (num_components) {
        case 1:
            internal_format = GL_R8UI;
            texture->input_format = GL_RED_INTEGER;
            break;
        case 4:
            internal_format = GL_RGBA8;
            texture->input_format = GL_RGBA;
            break;
        default:
            break;
    }

    glGenTextures(1, &texture->handle);
    glBindTexture(GL_TEXTURE_2D, texture->handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, texture->input_format, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void texture_free(Texture* texture)
{
    glDeleteTextures(1, &texture->handle);
}

void texture_set_pixels(Texture* texture, const u8* pixels)
{
    glBindTexture(GL_TEXTURE_2D, texture->handle);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture->w, texture->h, texture->input_format, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}
