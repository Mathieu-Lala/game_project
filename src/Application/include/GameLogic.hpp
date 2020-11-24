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

public: // signals // note : should be only related to input (elapsed time / keyboard / joysticks ...)

    // note : should have only one signal : onUserMove, or something like that
    // entt::sigh<void(entt::registry &, entt::entity &, const engine::d2::Acceleration &)> joystickMovement;
    entt::sigh<void(entt::registry &, entt::entity &, const Direction &dir, bool is_pressed)> onMovement;

    entt::sigh<void(entt::registry &)> onGameStart;

    entt::sigh<void(entt::registry &, entt::entity, const Class &)> onPlayerPurchase;

    entt::sigh<void(entt::registry &, entt::entity player)> onPlayerLevelUp;

    entt::sigh<void(entt::registry &, const engine::TimeElapsed &)> onGameUpdate;
    entt::sigh<void(entt::registry &, const engine::TimeElapsed &)> onGameUpdateAfter;

    entt::sigh<void(entt::registry &, entt::entity, const glm::dvec2 &, Spell &)> onSpellCast;

    entt::sigh<void(entt::registry &, entt::entity killed, entt::entity killer)> onEntityKilled;

    entt::sigh<void(entt::registry &)> onFloorChange;

private: // slots

    decltype(onMovement)::sink_type sinkMovement{onMovement};
    auto slots_move(entt::registry &, entt::entity &, const Direction &, bool is_pressed) -> void;

    //decltype(joystickMovement)::sink_type sinkJoystickMovement{joystickMovement};
    //auto joystickMove(entt::registry &world, entt::entity &player, const engine::d2::Acceleration &accel) -> void;

    decltype(onGameStart)::sink_type sinkOnGameStarted{onGameStart};
    auto slots_game_start(entt::registry &) -> void;

    decltype(onPlayerPurchase)::sink_type sinkOnPlayerBuyClass{onPlayerPurchase};
    auto slots_apply_classes(entt::registry &, entt::entity, const Class &) -> void;
    auto slots_purchase_classes(entt::registry &, entt::entity, const Class &) -> void;

    decltype(onPlayerLevelUp)::sink_type sinkOnPlayerLevelUp{onPlayerLevelUp};
    auto slots_level_up(entt::registry &, entt::entity) -> void;

    decltype(onGameUpdate)::sink_type sinkGameUpdated{onGameUpdate};
    auto slots_update_player_movement(entt::registry &, const engine::TimeElapsed &) -> void;
    auto slots_update_ai_movement(entt::registry &, const engine::TimeElapsed &) -> void;
    auto slots_update_ai_attack(entt::registry &, const engine::TimeElapsed &) -> void;
    auto slots_update_particle(entt::registry &, const engine::TimeElapsed &) -> void;
    // note : should be in Core
    auto slots_update_cooldown(entt::registry &, const engine::TimeElapsed &) -> void;
    auto slots_check_collision(entt::registry &, const engine::TimeElapsed &) -> void;
    auto slots_check_floor_change(entt::registry &, const engine::TimeElapsed &) -> void;

    /*[[deprecated]]*/ auto slots_update_effect(entt::registry &, const engine::TimeElapsed &) -> void;

    auto boss_anim_update(entt::registry &, const engine::TimeElapsed &) -> void; // todo : cleaner
    auto player_anim_update(entt::registry &, const engine::TimeElapsed &) -> void; // todo : cleaner

    decltype(onGameUpdateAfter)::sink_type sinkAfterGameUpdated{onGameUpdateAfter};
    auto slots_update_camera(entt::registry &, const engine::TimeElapsed &) -> void; // todo : cleaner

    decltype(onSpellCast)::sink_type sinkCastSpell{onSpellCast};
    auto slots_cast_spell(entt::registry &, entt::entity, const glm::dvec2 &, Spell &) -> void;

    decltype(onEntityKilled)::sink_type sinkGetKilled{onEntityKilled};
    auto slots_kill_entity(entt::registry &, entt::entity killed, entt::entity killer) -> void;

    decltype(onFloorChange)::sink_type sinkOnFloorChange{onFloorChange};
    auto slots_change_floor(entt::registry &) -> void;
};

} // namespace game
