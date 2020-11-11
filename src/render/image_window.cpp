#include "render/image_window.h"

#include <imgui.h>

void ImageWindow::draw(const char *title)
{
    ImGui::SetNextWindowSize({0.0f, 0.0f});
    ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(m_texture.handle)), ImVec2(400.0f, 400.0f));
    ImGui::End();
}

ImageWindow::~ImageWindow()
{
    texture_free(&m_texture);
}
