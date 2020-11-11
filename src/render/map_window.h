#ifndef KORLOW_MAP_WINDOW_H
#define KORLOW_MAP_WINDOW_H

#include "render/image_window.h"

struct PpuMapProxy;

class MapWindow : public ImageWindow {
public:
    MapWindow(PpuMapProxy *proxy);
    void update() override;

private:
    PpuMapProxy *m_proxy;
};

#endif    // KORLOW_MAP_WINDOW_H
