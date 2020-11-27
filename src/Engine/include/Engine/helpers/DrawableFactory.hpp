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
struct VBOTexture;

struct DrawableFactory {
    static auto rectangle() -> Drawable;

    static auto fix_color(entt::registry &, entt::entity, glm::vec3 &&color) -> Color &;
    static auto fix_texture(
        entt::registry &,
        entt::entity,
        const std::string_view filepath,
        const std::array<float, 4ul> &clip = {0.0f, 0.0f, 1.0f, 1.0f}) -> VBOTexture &;

    static auto fix_spritesheet(entt::registry &world, entt::entity entity, const std::string_view animation) -> void;
};

} // namespace engine
