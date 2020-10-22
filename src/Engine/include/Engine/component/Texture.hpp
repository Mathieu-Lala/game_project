#pragma once

#include <cstdint>
#include <array>
#include <string_view>

namespace engine {

struct Texture {

    std::array<float, 8ul> vertices = {
        0.0f, 1.0f, // top left
        1.0f, 1.0f, // top right
        0.0f, 0.0f, // bottom left
        1.0f, 0.0f, // bottom right
    };

    std::uint32_t VBO;
    std::uint32_t texture;

    std::int32_t width;
    std::int32_t height;
    std::int32_t channels;
    std::uint8_t *px;

    static auto ctor(const std::string_view path, std::array<float, 4ul> &&) -> Texture;

    static auto dtor(Texture *ptr) -> void;

};

} // namespace engine
