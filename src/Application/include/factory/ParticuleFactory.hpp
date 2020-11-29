#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "component/Particule.hpp"

namespace game {

struct ParticuleFactory {
    template<Particule::ID>
    static auto create(entt::registry &, const glm::vec2 &pos, const glm::vec3 &) -> void;
};

#define DECL_SPEC(id) \
    template<>        \
    auto ParticuleFactory::create<Particule::ID::id>(entt::registry &, const glm::vec2 &, const glm::vec3 &)->void

DECL_SPEC(POSITIVE);
DECL_SPEC(HITMARKER);
DECL_SPEC(NEUTRAL);

#undef DECL_SPEC

} // namespace game
