#pragma once

#include <cctype>
#include <magic_enum.hpp>
#include <entt/entt.hpp>
#include <glm/vec2.hpp>

#include "models/Enemy.hpp"

namespace game {

class ThePURGE;

struct EntityFactory {
    enum class Layer {
        ERROR,
        PARTICULE,
        LAYER_PLAYER,
        LAYER_ENEMY,
        LAYER_DEBUG,
        LAYER_TERRAIN,
        LAYER_BACKGROUND,
        MAX_LAYER,
    };

    enum ID {
        DEBUG_TILE,
        AIMING_SIGHT,
        PLAYER,
        KEY,
        FLOOR_NORMAL,
        FLOOR_SPAWN,
        FLOOR_BOSS,
        FLOOR_CORRIDOR,
        EXIT_DOOR,
        WALL,
        BACKGROUND,
    };

    // note : should take a path to a json config instead ... ?
    // todo : normalize arguments , Args...&& ?
    template<ID>
    /*[[deprecated]]*/ static auto create(ThePURGE &, entt::registry &, const glm::vec2 &pos, const glm::vec2 &size)
        -> entt::entity;

    template<Layer layer>
    constexpr static auto get_z_layer() noexcept -> double
    {
        return static_cast<double>(layer);
    }

    static auto create(ThePURGE &, entt::registry &, const glm::vec2 &pos, const Enemy &) -> entt::entity;
};

#define DECL_SPEC(id)                                                       \
    template<>                                                              \
    auto /*[[deprecated]]*/ EntityFactory::create<EntityFactory::ID::id>(   \
        ThePURGE &, entt::registry &, const glm::vec2 &, const glm::vec2 &) \
        ->entt::entity

DECL_SPEC(DEBUG_TILE);

DECL_SPEC(FLOOR_NORMAL);
DECL_SPEC(FLOOR_SPAWN);
DECL_SPEC(FLOOR_BOSS);
DECL_SPEC(FLOOR_CORRIDOR);
DECL_SPEC(EXIT_DOOR);
DECL_SPEC(WALL);

DECL_SPEC(AIMING_SIGHT);
DECL_SPEC(PLAYER);
DECL_SPEC(KEY);

#undef DECL_SPEC

} // namespace game
