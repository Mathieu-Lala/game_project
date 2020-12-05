#include <Engine/Graphics/Window.hpp>
#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/helpers/TextureLoader.hpp>
#include <Engine/Core.hpp>

#include "widgets/helpers.hpp"

namespace game::helper {

auto getTexture(const std::string &simplePath) -> std::uint32_t
{
    static auto holder = engine::Core::Holder{};

    const auto &dataFolder = holder.instance->settings().data_folder;

    return engine::helper::loadTexture(dataFolder + simplePath);
}

auto from1080p(float x, float y) noexcept -> ImVec2 { return ImVec2{x / 1920.0f, y / 1080.0f}; }

auto frac2pixel(ImVec2 fraction) noexcept -> ImVec2
{
    const auto winSize = engine::Core::Holder{}.instance->window()->getSize();
    return ImVec2(static_cast<float>(winSize.x) * fraction.x, static_cast<float>(winSize.y) * fraction.y);
}

void drawTexture(std::uint32_t id, ImVec2 topLeft, ImVec2 size) noexcept
{
    ImGui::SetCursorPos(topLeft);
    ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(id)), size);
}

void drawTexture(const GUITexture &t) noexcept { drawTexture(t.id, frac2pixel(t.topleft), frac2pixel(t.size)); }

void drawText(ImVec2 pos, const std::string &str, ImVec4 color, ImFont *font) noexcept
{
    if (font) ImGui::PushFont(font);
    ImGui::SetCursorPos(pos);
    ImGui::TextColored(color, "%s", str.c_str());
    if (font) ImGui::PopFont();
}

void drawTextWrapped(ImVec2 pos, const std::string &str, float maxX, ImFont *font) noexcept
{
    if (font) ImGui::PushFont(font);
    ImGui::SetCursorPos(pos);
    ImGui::PushTextWrapPos(maxX);

    ImGui::TextWrapped("%s", str.c_str());

    ImGui::PopTextWrapPos();
    if (font) ImGui::PopFont();
}

} // namespace game::helper
