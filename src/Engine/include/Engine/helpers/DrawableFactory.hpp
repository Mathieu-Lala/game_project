#pragma once

#include <cassert>

#include <glm/vec3.hpp>

#include <entt/entt.hpp>

namespace engine {

struct Drawable;
struct Color;
struct Texture;

struct DrawableFactory {
    static auto rectangle() -> Drawable;

    static auto fix_color(entt::registry &, entt::entity, glm::vec3 &&color) -> Color &;
    static auto fix_texture(entt::registry &, entt::entity, const std::string_view filepath) -> Texture &;
};

} // namespace engine
