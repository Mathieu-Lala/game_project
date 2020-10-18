#pragma once

#include <cassert>

#include <glm/vec3.hpp>

#include <entt/entt.hpp>

namespace engine {

struct Drawable;
struct Color;

struct DrawableFactory {
    static auto rectangle() -> Drawable;

    static auto fix_color(entt::registry &world, entt::entity e, glm::vec3 &&color) -> Color &;
};

} // namespace engine
