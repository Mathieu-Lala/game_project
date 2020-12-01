#include <Engine/Core.hpp>
#include <Engine/Graphics/Window.hpp>

#include "widgets/helpers.hpp"

namespace game::helpers {

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

} // namespace game::helpers
