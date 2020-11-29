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

    static auto ctor(const std::string_view filepath) -> Texture
    {
        Texture texture = {
            .id = 0,
            .width = 0,
            .height = 0,
            .channels = 0,
            .px = nullptr,
        };

        texture.px = ::stbi_load(filepath.data(), &texture.width, &texture.height, &texture.channels, 4);
        if (texture.px == nullptr)
            spdlog::error("Could not open texture '{}'. Texture will appear black", filepath.data());

        ::glGenTextures(1, &texture.id);
        ::glBindTexture(GL_TEXTURE_2D, texture.id);

        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //
        //    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.px);
        ::glGenerateMipmap(GL_TEXTURE_2D);

        return texture;
    }

    static auto dtor(Texture *obj)
    {
        ::stbi_image_free(obj->px);
        ::glDeleteTextures(1, &obj->id);
    }
};

} // namespace engine
