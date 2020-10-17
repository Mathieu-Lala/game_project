#pragma once

#include <cassert>

#include <glm/vec3.hpp>

#include <entt/entt.hpp>

#include "Engine/resources/LoaderTexture.hpp"

namespace engine {

struct Drawable;
struct Color;

struct DrawableFactory {
    static auto rectangle() -> Drawable;

    static auto fix_color(entt::registry &world, entt::entity e, glm::vec3 &&color) -> Color &;
    static auto fix_texture(entt::registry &world, entt::entity e, const std::string_view filepath) -> Texture &;
};

} // namespace engine
