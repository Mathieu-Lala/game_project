#pragma once

#include <iostream>

#include <glm/vec2.hpp>

#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

#include <Engine/Event/Event.hpp>

#include "component/all.hpp"
#include "level/MapGenerator.hpp"
#include "models/Class.hpp"

namespace game {

class ThePURGE;

enum class Direction {
    UP,
    LEFT,
    DOWN,
    RIGHT
};

class GameLogic {
public:
    ThePURGE &m_game;

    GameLogic(ThePURGE &game);

    entt::sigh<void(entt::registry &, entt::entity &, const Direction &dir)> movement;
    entt::sink<void(entt::registry &, entt::entity &, const Direction &dir)> sinkMovement{movement};

    entt::sigh<void(entt::registry &, entt::entity &, const engine::d2::Acceleration &)> joystickMovement;
    entt::sink<void(entt::registry &, entt::entity &, const engine::d2::Acceleration &)> sinkJoystickMovement{joystickMovement};

    entt::sigh<void(entt::registry &)> onGameStarted;
    entt::sink<void(entt::registry &)> sinkOnGameStarted{onGameStarted};

    entt::sigh<void(entt::registry &, entt::entity player, const Class &)> onPlayerBuyClass;
    entt::sink<void(entt::registry &, entt::entity player, const Class &)> sinkOnPlayerBuyClass{onPlayerBuyClass};

    entt::sigh<void(entt::registry &, entt::entity player)> onPlayerLevelUp;
    entt::sink<void(entt::registry &, entt::entity player)> sinkOnPlayerLevelUp{onPlayerLevelUp};

    // entityLogic signal loop
    entt::sigh<void(entt::registry &, const engine::TimeElapsed &)> gameUpdated;
    entt::sink<void(entt::registry &, const engine::TimeElapsed &)> sinkGameUpdated{gameUpdated};

    entt::sigh<void(entt::registry &, entt::entity, const glm::dvec2 &, Spell &)> castSpell;
    entt::sink<void(entt::registry &, entt::entity, const glm::dvec2 &, Spell &)> sinkCastSpell{castSpell};

    entt::sigh<void(entt::registry &, entt::entity killed, entt::entity killer)> playerKilled;
    entt::sink<void(entt::registry &, entt::entity killed, entt::entity killer)> sinkGetKilled{playerKilled};

    entt::sigh<void(entt::registry &)> onFloorChange;
    entt::sink<void(entt::registry &)> sinkOnFloorChange{onFloorChange};


    auto move(entt::registry &world, entt::entity &player, const Direction &dir) -> void;
    auto joystickMove(entt::registry &world, entt::entity &player, const engine::d2::Acceleration &accel) -> void;

    auto on_game_started(entt::registry &world) -> void;

    auto apply_class_to_player(entt::registry &world, entt::entity player, const Class &) -> void;
    auto on_class_bought(entt::registry &world, entt::entity player, const Class &) -> void;

    auto on_player_level_up(entt::registry &world, entt::entity player) -> void;

    auto ai_pursue(entt::registry &world, const engine::TimeElapsed &dt) -> void;

    auto update_lifetime(entt::registry &world, const engine::TimeElapsed &dt) -> void;
    auto update_particule(entt::registry &world, const engine::TimeElapsed &dt) -> void;

    auto cooldown(entt::registry &world, const engine::TimeElapsed &dt) -> void;

    auto enemies_try_attack(entt::registry &world, const engine::TimeElapsed &dt) -> void;

    auto check_collision(entt::registry &world, const engine::TimeElapsed &dt) -> void;

    auto effect(entt::registry &world, const engine::TimeElapsed &dt) -> void;

    auto exit_door_interraction(entt::registry &, const engine::TimeElapsed &dt) -> void;

    auto player_anim_update(entt::registry &, const engine::TimeElapsed &dt) -> void;

    auto entity_killed(entt::registry &, entt::entity killed, entt::entity killer) -> void;

    auto cast_attack(entt::registry &, entt::entity, const glm::dvec2 &, Spell &) -> void;

    auto goToTheNextFloor(entt::registry &world) -> void;

    auto addXp(entt::registry &world, entt::entity player, std::uint32_t xp) -> void;

    FloorGenParam m_map_generation_params;
    std::uint32_t m_nextFloorSeed;

    static constexpr double kDoorInteractionRange = 3;
};

} // namespace game
