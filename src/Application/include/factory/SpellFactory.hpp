#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

namespace game {

struct SpellData;
struct SpellDatabase;

struct SpellFactory {
    static auto create(SpellDatabase &, entt::registry &, entt::entity caster, const glm::dvec2 &direction, const SpellData &)
        -> entt::entity;
};

} // namespace game
