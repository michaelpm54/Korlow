#include "render/map_window.h"

#include "ppu_map_proxy.h"

MapWindow::MapWindow(PpuMapProxy *proxy)
    : m_proxy(proxy)
{
    texture_init(&m_texture, 32, 64, 4);
}

void MapWindow::update()
{
    texture_set_pixels(&m_texture, m_proxy->get_map_pixels());
}
