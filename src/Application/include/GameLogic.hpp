#pragma once

#include <iostream>

#include <glm/vec2.hpp>

#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

#include <Engine/Event/Event.hpp>

#include "component/all.hpp"

using namespace std::chrono_literals; // ms ..

namespace game {

class ThePurge;

class GameLogic {
public:
    ThePurge &m_game;

    GameLogic(ThePurge &game);

    // Init movement signal player
    entt::sigh<void(entt::registry &, entt::entity &, const engine::d2::Acceleration &)> movement;
    entt::sink<void(entt::registry &, entt::entity &, const engine::d2::Acceleration &)> sinkMovement{movement};

    // entityLogic signal loop
    entt::sigh<void(entt::registry &, const engine::TimeElapsed &)> gameUpdated;
    entt::sink<void(entt::registry &, const engine::TimeElapsed &)> sinkGameUpdated{gameUpdated};

    entt::sigh<void(entt::registry &, entt::entity, const glm::dvec2 &)> castSpell;
    entt::sink<void(entt::registry &, entt::entity, const glm::dvec2 &)> sinkCastSpell{castSpell};

    entt::sigh<void(entt::registry &, entt::entity killed, entt::entity killer)> playerKilled;
    entt::sink<void(entt::registry &, entt::entity killed, entt::entity killer)> sinkGetKilled{playerKilled};

    auto move(entt::registry &world, entt::entity &player, const engine::d2::Acceleration &accel) -> void;

    auto ai_pursue(entt::registry &world, const engine::TimeElapsed &dt) -> void;
    auto update_lifetime(entt::registry &world, const engine::TimeElapsed &dt) -> void;
    auto cooldown(entt::registry &world, const engine::TimeElapsed &dt) -> void;
    auto enemies_try_attack(entt::registry &world, const engine::TimeElapsed &dt) -> void;
    auto check_collision(entt::registry &world, const engine::TimeElapsed &dt) -> void;
    //auto effect(entt::registry &world, entt::entity &player, const engine::TimeElapsed &dt) -> void;
    auto entity_killed(entt::registry &, entt::entity killed, entt::entity killer) -> void;

    auto cast_attack(entt::registry &world, entt::entity entity, const glm::dvec2 &direction) -> void;

};

} // namespace game
