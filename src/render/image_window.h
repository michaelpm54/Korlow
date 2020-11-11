#ifndef KORLOW_IMAGE_WINDOW_H
#define KORLOW_IMAGE_WINDOW_H

#include "render/gl_texture.h"

class ImageWindow {
public:
    virtual ~ImageWindow();
    virtual void update() = 0;
    void draw(const char *title);

protected:
    Texture m_texture;
};

#endif    // KORLOW_IMAGE_WINDOW_H
