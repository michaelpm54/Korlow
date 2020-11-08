#ifndef KORLOW_SCREEN_TEXTURE_H
#define KORLOW_SCREEN_TEXTURE_H

#include "emu_types.h"
#include "render/gl.h"

struct Texture {
    int w;
    int h;
    GLenum input_format;
    GLuint handle;
};

void texture_init(Texture* texture, int w, int h, int num_components);
void texture_free(Texture* texture);
void texture_set_pixels(Texture* texture, const u8* pixels);

#endif    // KORLOW_SCREEN_TEXTURE_H
