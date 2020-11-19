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

    // todo : should be a signals slots too !
    auto addXp(entt::registry &, entt::entity player, std::uint32_t xp) -> void;

    FloorGenParam m_map_generation_params; // note : should be private

private:

    std::uint32_t m_nextFloorSeed;

    static constexpr double kDoorInteractionRange = 3;

public: // signals

    // note : should have only one signal : onUserMove, or something like that
    entt::sigh<void(entt::registry &, entt::entity &, const engine::d2::Acceleration &)> joystickMovement;
    entt::sigh<void(entt::registry &, entt::entity &, const Direction &dir)> movement;

    entt::sigh<void(entt::registry &)> onGameStarted;

    entt::sigh<void(entt::registry &, entt::entity player, const Class &)> onPlayerBuyClass;

    entt::sigh<void(entt::registry &, entt::entity player)> onPlayerLevelUp;

    entt::sigh<void(entt::registry &, const engine::TimeElapsed &)> gameUpdated;
    entt::sigh<void(entt::registry &, const engine::TimeElapsed &)> afterGameUpdated;

    entt::sigh<void(entt::registry &, entt::entity, const glm::dvec2 &, Spell &)> castSpell;

    entt::sigh<void(entt::registry &, entt::entity killed, entt::entity killer)> onEntityKilled;

    entt::sigh<void(entt::registry &)> onFloorChange;

private: // slots

    decltype(movement)::sink_type sinkMovement{movement};
    auto move(entt::registry &, entt::entity &, const Direction &) -> void;

    decltype(joystickMovement)::sink_type sinkJoystickMovement{joystickMovement};
    auto joystickMove(entt::registry &world, entt::entity &player, const engine::d2::Acceleration &accel) -> void;

    decltype(onGameStarted)::sink_type sinkOnGameStarted{onGameStarted};
    auto on_game_started(entt::registry &) -> void;

    decltype(onPlayerBuyClass)::sink_type sinkOnPlayerBuyClass{onPlayerBuyClass};
    auto apply_class_to_player(entt::registry &, entt::entity, const Class &) -> void;
    auto on_class_bought(entt::registry &, entt::entity, const Class &) -> void;

    decltype(onPlayerLevelUp)::sink_type sinkOnPlayerLevelUp{onPlayerLevelUp};
    auto on_player_level_up(entt::registry &, entt::entity) -> void;

    decltype(gameUpdated)::sink_type sinkGameUpdated{gameUpdated};
    auto player_movement_update(entt::registry &, const engine::TimeElapsed &) -> void;
    auto ai_pursue(entt::registry &, const engine::TimeElapsed &) -> void;
    auto enemies_try_attack(entt::registry &, const engine::TimeElapsed &) -> void;
    auto update_lifetime(entt::registry &, const engine::TimeElapsed &) -> void;
    auto update_particule(entt::registry &, const engine::TimeElapsed &) -> void;
    auto cooldown(entt::registry &, const engine::TimeElapsed &) -> void;
    auto check_collision(entt::registry &, const engine::TimeElapsed &) -> void;
    auto effect(entt::registry &, const engine::TimeElapsed &) -> void;
    auto exit_door_interraction(entt::registry &, const engine::TimeElapsed &) -> void;
    auto boss_anim_update(entt::registry &, const engine::TimeElapsed &) -> void;
    auto player_anim_update(entt::registry &, const engine::TimeElapsed &) -> void;

    decltype(afterGameUpdated)::sink_type sinkAfterGameUpdated{afterGameUpdated};
    auto update_camera(entt::registry &, const engine::TimeElapsed &) -> void;

    decltype(castSpell)::sink_type sinkCastSpell{castSpell};
    auto cast_attack(entt::registry &, entt::entity, const glm::dvec2 &, Spell &) -> void;

    decltype(onEntityKilled)::sink_type sinkGetKilled{onEntityKilled};
    auto entity_killed(entt::registry &, entt::entity killed, entt::entity killer) -> void;

    decltype(onFloorChange)::sink_type sinkOnFloorChange{onFloorChange};
    auto goToTheNextFloor(entt::registry &) -> void;
};

} // namespace game
