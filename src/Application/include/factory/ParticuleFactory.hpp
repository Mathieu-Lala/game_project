#pragma once

#include <entt/entt.hpp>

#include "component/Particule.hpp"

namespace game {

struct ParticuleFactory {
    template<Particule::ID>
    static auto create(entt::registry &, const glm::vec2 &pos) -> void;
};

#define DECL_SPEC(id) \
    template<>        \
    auto ParticuleFactory::create<Particule::ID::id>(entt::registry &, const glm::vec2 &) -> void

DECL_SPEC(HITMARKER);

#undef DECL_SPEC

} // namespace game
