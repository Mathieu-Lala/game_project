#pragma once

#include <magic_enum.hpp>
#include <entt/entt.hpp>
#include <glm/vec2.hpp>

namespace game {

struct EntityFactory {
    enum ID {
        ID_ERROR = 0,

        LAYER_DEBUG = 1,
        DEBUG_TILE = LAYER_DEBUG * 10,

        LAYER_PLAYER = 2,
        FARMER = LAYER_PLAYER * 10,
        SOLDIER,
        SHOOTER,
        SORCERER,
        TONK,
        PLAYER, // should be named farmer

        KEY, // should be on a layer above


        LAYER_ENEMY = 3,
        ENEMY = LAYER_ENEMY * 10,
        BOSS,


        LAYER_TERRAIN = 4,

        FLOOR_NORMAL = LAYER_TERRAIN * 10,
        FLOOR_SPAWN,
        FLOOR_BOSS,
        FLOOR_CORRIDOR,
        EXIT_DOOR,
        WALL,

        LAYER_BACKGROUND,
        BACKGROUND,

        MAX_ID,
    };

    static auto toID(const std::string_view in) noexcept -> ID
    {
        std::string compare{in};
        std::transform(compare.begin(), compare.end(), compare.begin(), [](auto c) { return static_cast<char>(std::tolower(c)); });

        for (const auto &i : magic_enum::enum_values<ID>()) {
            auto enum_name = std::string{magic_enum::enum_name(i)};
            std::transform(enum_name.begin(), enum_name.end(), enum_name.begin(), [](auto c) { return static_cast<char>(std::tolower(c)); });
            if (compare == enum_name) { return static_cast<ID>(i); }
        }
        return LAYER_DEBUG;
    }

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

DECL_SPEC(BACKGROUND);


#undef DECL_SPEC

} // namespace game
