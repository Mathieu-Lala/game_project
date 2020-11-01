#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

namespace game {

struct EntityFactory {
    enum ID {
        LAYER_DEBUG,

        DEBUG_TILE,


        LAYER_PLAYER,
        PLAYER, // should be named farmer
        KEY,


        LAYER_ENEMY,
        BOSS,
        ENEMY,


        LAYER_TERRAIN,

        FLOOR_NORMAL,
        FLOOR_SPAWN,
        FLOOR_BOSS,
        FLOOR_CORRIDOR,
        EXIT_DOOR,
        WALL,

        MAX_ID,
    };

    // note : should take a path to a json config instead ... ?
    // todo : normalize arguments , Args...&& ?
    template<ID>
    static auto create(entt::registry &, const glm::vec2 &pos, const glm::vec2 &size /*, float rotation*/) -> entt::entity;

    template<ID id>
    constexpr static auto get_z_layer() noexcept -> double
    {
        return static_cast<double>(id);
    }

};

#define DECL_SPEC(id)                                                                \
    template<>                                                                       \
    auto EntityFactory::create<EntityFactory::ID::id>(                               \
        entt::registry &, const glm::vec2 &, const glm::vec2 & /*, float rotation*/) \
        ->entt::entity

DECL_SPEC(DEBUG_TILE);

DECL_SPEC(FLOOR_NORMAL);
DECL_SPEC(FLOOR_SPAWN);
DECL_SPEC(FLOOR_BOSS);
DECL_SPEC(FLOOR_CORRIDOR);
DECL_SPEC(EXIT_DOOR);
DECL_SPEC(WALL);

DECL_SPEC(BOSS);
DECL_SPEC(ENEMY);

DECL_SPEC(PLAYER);
DECL_SPEC(KEY);

#undef DECL_SPEC

} // namespace game
