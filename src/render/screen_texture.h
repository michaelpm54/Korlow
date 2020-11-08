#ifndef KORLOW_SCREEN_TEXTURE_H
#define KORLOW_SCREEN_TEXTURE_H

#include <glad/glad.h>

#include "emu_types.h"

void texture_init(GLuint* texture, int w, int h);
void texture_free(GLuint* texture);
void texture_set_pixels(GLuint texture, const u8* pixels, int w, int h);

#endif    // KORLOW_SCREEN_TEXTURE_H
