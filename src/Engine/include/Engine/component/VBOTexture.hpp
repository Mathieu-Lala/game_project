#pragma once

#include <cstdint>
#include <array>
#include <string_view>

namespace engine {

struct VBOTexture {
    // clang-format off
    std::array<float, 8ul> vertices = {
        0.0f, 1.0f, // top left
        1.0f, 1.0f, // top right
        0.0f, 0.0f, // bottom left
        1.0f, 0.0f, // bottom right
    };
    // clang-format on

    std::uint32_t VBO;
    std::uint32_t id;
    bool mirrored;

    static auto ctor(const std::string_view path, bool mirrored_repeated, const std::array<float, 4ul> &) -> VBOTexture;

    static auto dtor(VBOTexture *ptr) -> void;
};

} // namespace engine
