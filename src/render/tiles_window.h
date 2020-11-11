#ifndef KORLOW_TILES_WINDOW_H
#define KORLOW_TILES_WINDOW_H

#include "render/image_window.h"

class TilesWindow : public ImageWindow {
public:
    TilesWindow(u8 *mem, u8 *palette);
    void update() override;

private:
    u8 *m_mem;
    u8 *m_pal;
};

#endif    // KORLOW_TILES_WINDOW_H
