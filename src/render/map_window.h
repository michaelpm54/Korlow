#ifndef KORLOW_MAP_WINDOW_H
#define KORLOW_MAP_WINDOW_H

#include "render/image_window.h"

struct Ppu;

class MapWindow : public ImageWindow {
public:
    MapWindow(Ppu *ppu);
    void update() override;

private:
    Ppu *m_ppu;
};

#endif    // KORLOW_MAP_WINDOW_H
