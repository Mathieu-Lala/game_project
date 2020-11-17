#pragma once

#include <cassert>
#include <cstdint>
#include <array>
#include <stdexcept>

#include <glm/vec3.hpp>

#include <entt/entt.hpp>

#include <Engine/Graphics/third_party.hpp>
#include <stb_image.h>

namespace engine {

struct Drawable;
struct Color;
struct Texture;

struct DrawableFactory {
    static auto rectangle() -> Drawable;

    static auto fix_color(entt::registry &, entt::entity, glm::vec3 &&color) -> Color &;
    static auto fix_texture(
        entt::registry &,
        entt::entity,
        const std::string_view filepath,
        const std::array<float, 4ul> &clip = {0.0f, 0.0f, 1.0f, 1.0f}) -> Texture &;

    // todo : cleaner
    static GLuint createtexture(const std::string &fullpath)
    {
        int w, h, channel;
        auto image = stbi_load(fullpath.c_str(), &w, &h, &channel, 4);

        if (!image) { throw std::runtime_error("Failed to load image " + fullpath + " : " + stbi_failure_reason()); }

        GLuint id;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);

        return id;
    }

};

} // namespace engine
