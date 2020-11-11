#include "render/message_queue.h"

#include <imgui.h>

static constexpr int kMessageBoxFlags {ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing};

void MessageQueue::push(const std::string& msg, std::chrono::duration<int> duration)
{
    m_messages.push_back({msg, std::chrono::steady_clock::now() + duration});
}

void MessageQueue::update()
{
    if (m_messages.empty())
        return;

    const auto now = std::chrono::steady_clock::now();

    m_messages.erase(
        std::remove_if(
            m_messages.begin(),
            m_messages.end(),
            [&now](const auto& message) {
                return now > message.expire;
            }),
        m_messages.end());
}

void MessageQueue::draw()
{
    if (m_messages.empty())
        return;

    ImGui::SetNextWindowPos({500, 40});
    ImGui::SetNextWindowSize({260, 0});
    ImGui::Begin("Messages", nullptr, kMessageBoxFlags);
    for (auto const& message : m_messages) {
        ImGui::TextWrapped("%s", message.str.c_str());
    }
    ImGui::End();
}
