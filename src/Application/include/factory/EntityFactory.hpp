#pragma once

#include <cctype>
#include <magic_enum.hpp>
#include <entt/entt.hpp>
#include <glm/vec2.hpp>

#include "models/Enemy.hpp"

//#include "models/utils.hpp"

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

    //    enum ID {
    //        ID_ERROR = 0,
    //
    //        LAYER_DEBUG = 1,
    //         = LAYER_DEBUG * 10,
    //
    //        LAYER_AIMING_SIGHT = 2,
    //
    //        LAYER_PLAYER = 3,
    //
    //
    //        LAYER_ENEMY = 5,
    //
    //        LAYER_TERRAIN = 6,
    //
    //         = LAYER_TERRAIN * 10,

    //
    //        LAYER_BACKGROUND = 7,
    //
    //        MAX_ID,
    //    };

    //static auto toID(const std::string_view in) noexcept -> ID
    //{
    //    std::string compare{in};
    //    std::transform(
    //        compare.begin(), compare.end(), compare.begin(), [](auto c) { return static_cast<char>(std::tolower(c)); });
//
    //    for (const auto &i : magic_enum::enum_values<ID>()) {
    //        auto enum_name = std::string{magic_enum::enum_name(i)};
    //        std::transform(enum_name.begin(), enum_name.end(), enum_name.begin(), [](auto c) {
    //            return static_cast<char>(std::tolower(c));
    //        });
    //        if (compare == enum_name) { return static_cast<ID>(i); }
    //    }
    //    return LAYER_DEBUG;
    //}

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
