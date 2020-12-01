#pragma once

#include <imgui.h>
#include <cstdint>

namespace game {

struct GUITexture {
    std::uint32_t id;

    // In a range [0; 1] relative to the window size
    ImVec2 topleft;
    ImVec2 size;
};

namespace helpers {

auto frac2pixel(ImVec2 fraction) noexcept -> ImVec2;

// topLeft and size in PIXELS
void drawTexture(std::uint32_t id, ImVec2 topLeft, ImVec2 size) noexcept;

void drawTexture(const GUITexture &t) noexcept;
} // namespace helpers

} // namespace game