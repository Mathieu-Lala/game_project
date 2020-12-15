#pragma once

#include <string_view>
#include <cstdint>

#include <spdlog/spdlog.h>
#include <stb_image.h>

#include "Engine/Graphics/third_party.hpp"


namespace engine {

struct Texture {
    GLuint id;

    std::int32_t width;
    std::int32_t height;
    std::int32_t channels;
    std::uint8_t *px;

    static auto ctor(const std::string_view filepath, bool mirrored_repeated) -> Texture;

    static auto dtor(Texture *obj) -> void;
};

} // namespace engine
