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

namespace helper {

// path relative to the data directory
auto getTexture(const std::string &simplePath) -> std::uint32_t;
auto from1080p(float x, float y) noexcept -> ImVec2;

auto frac2pixel(ImVec2 fraction) noexcept -> ImVec2;

// topLeft and size in PIXELS
void drawTexture(std::uint32_t id, ImVec2 topLeft, ImVec2 size, ImVec4 tintColor = ImVec4(1, 1, 1, 1)) noexcept;

void drawTexture(const GUITexture &t, ImVec4 tintColor = ImVec4(1, 1, 1, 1)) noexcept;

void drawText(ImVec2 pos, const std::string &str, ImVec4 color = ImVec4(1, 1, 1, 1), ImFont *font = nullptr) noexcept;
void drawTextWrapped(ImVec2 pos, const std::string &str, float maxX, ImFont *font = nullptr) noexcept;

} // namespace helper

} // namespace game