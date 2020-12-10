#include <spdlog/spdlog.h>
#include <sstream>

#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Event/Event.hpp>
#include <Engine/audio/AudioManager.hpp>
#include <Engine/Settings.hpp>
#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Core.hpp>


#include "models/Spell.hpp"
#include "models/Class.hpp"

#include "GameLogic.hpp"
#include "ThePURGE.hpp"
#include "factory/EntityFactory.hpp"
#include "factory/SpellFactory.hpp"
#include "factory/ParticuleFactory.hpp"

#include "component/all.hpp"

#include "menu/UpgradePanel.hpp"
#include "menu/GameOver.hpp"

using namespace std::chrono_literals;

auto game::GameLogic::slots_move([[maybe_unused]] entt::registry &world, entt::entity &player, const Direction &dir, bool is_pressed)
    -> void
{
    switch (dir) {
    case Direction::UP: world.get<ControllerAxis>(player).movement.y = is_pressed ? 1 : 0.0f; break;
    case Direction::DOWN: world.get<ControllerAxis>(player).movement.y = is_pressed ? -1 : 0.0f; break;
    case Direction::RIGHT: world.get<ControllerAxis>(player).movement.x = is_pressed ? 1 : 0.0f; break;
    case Direction::LEFT: world.get<ControllerAxis>(player).movement.x = is_pressed ? -1 : 0.0f; break;
    default: break;
    }

    const auto &movement = world.get<ControllerAxis>(player).movement;
    if (glm::length(movement) < 0.01f) return;

    const auto &direction = glm::normalize(movement);

    world.get<ControllerAxis>(player).aiming = direction;
    world.get<ControllerAxis>(player).movement = direction;
}

auto game::GameLogic::slots_update_player_movement(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt)
    -> void
{
    auto player = m_game.player;

    auto &vel = world.get<engine::d2::Velocity>(player);
    const auto &axis = world.get<ControllerAxis>(player);
    const auto &spd = world.get<Speed>(player).speed;

    vel.x = axis.movement.x * spd;
    vel.y = axis.movement.y * spd;
}

auto game::GameLogic::slots_update_ai_movement(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
{
    const auto pursue = [&world](entt::entity entity, entt::entity target, engine::d2::Velocity &out) {
        const auto &pos = world.get<engine::d3::Position>(entity);
        const auto &view_range = world.get<ViewRange>(entity);

        const auto &target_pos = world.get<engine::d3::Position>(target);
        const auto diff = glm::vec2{target_pos.x - pos.x, target_pos.y - pos.y};

        if (glm::length(diff) > view_range.range) return false;

        for (float i = 0.0f; i != 10.0f; i++) {
            const auto in_between =
                glm::vec2{static_cast<float>(pos.x) + i * diff.x / 10.0f, static_cast<float>(pos.y) + i * diff.y / 10.0f};
            for (const auto &wall : world.view<entt::tag<"wall"_hs>>()) {
                const auto &wall_pos = world.get<engine::d3::Position>(wall);
                const auto &wall_hitbox = world.get<engine::d2::HitboxSolid>(wall);

                if (engine::d2::overlapped<engine::d2::WITHOUT_EDGE>(
                        engine::d2::HitboxSolid{0.01, 0.01},
                        engine::d3::Position{in_between.x, in_between.y, 0.0f},
                        wall_hitbox,
                        wall_pos)) {
                    return false;
                }
            }
        }

        const auto &spd = world.get<Speed>(entity).speed;
        const auto result = glm::normalize(diff);
        out = {result.x * spd, result.y * spd};

        return true;
    };

    for (auto &i : world.view<entt::tag<"enemy"_hs>>()) {
        if (world.has<engine::Spritesheet>(i) && world.get<engine::Spritesheet>(i).current_animation == "death")
            continue;
        auto &vel = world.get<engine::d2::Velocity>(i);
        pursue(i, m_game.player, vel);
    }
}

auto game::GameLogic::slots_check_collision(entt::registry &world, const engine::TimeElapsed &) -> void
{
    // note : this is check at each frame but we only want it when the spell (or the wall) move
    for (auto &spell : world.view<entt::tag<"spell"_hs>, entt::tag<"projectile"_hs>>()) {
        const auto &spell_pos = world.get<engine::d3::Position>(spell);
        const auto &spell_box = world.get<engine::d2::HitboxFloat>(spell);

        for (const auto &wall : world.view<entt::tag<"wall"_hs>, engine::d2::HitboxSolid>()) {
            const auto &wall_pos = world.get<engine::d3::Position>(wall);
            const auto &wall_box = world.get<engine::d2::HitboxSolid>(wall);

            if (engine::d2::overlapped<engine::d2::WITH_EDGE>(spell_box, spell_pos, wall_box, wall_pos)) {
                world.destroy(spell);
                break;
            }
        }
    }

    const auto check_spell_collision = [this, &world](auto &entity, auto &spell, auto &source) {
        if (world.has<engine::Spritesheet>(entity) && world.get<engine::Spritesheet>(entity).current_animation == "death")
            return;

        const auto &spell_pos = world.get<engine::d3::Position>(spell);
        const auto &spell_hitbox = world.get<engine::d2::HitboxFloat>(spell);

        const auto &entity_pos = world.get<engine::d3::Position>(entity);
        const auto &entity_hitbox = world.get<engine::d2::HitboxSolid>(entity);

        if (engine::d2::overlapped<engine::d2::WITHOUT_EDGE>(entity_hitbox, entity_pos, spell_hitbox, spell_pos)) {
            onCollideWithSpell.publish(world, entity, source, spell);
        }
    };

    for (const auto &spell : world.view<entt::tag<"spell"_hs>>()) {
        const auto &source = world.get<engine::Source>(spell).source;
        if (!world.valid(source)) return;

        for (const auto &player : world.view<entt::tag<"player"_hs>, engine::d2::HitboxSolid>()) {
            if (!world.valid(spell)) break;
            check_spell_collision(player, spell, source);
        }
        for (const auto &enemy : world.view<entt::tag<"enemy"_hs>, engine::d2::HitboxSolid>()) {
            if (!world.valid(spell)) break;
            check_spell_collision(enemy, spell, source);
        }
    }

    world.view<KeyPicker, engine::d2::HitboxSolid, engine::d3::Position>().each([&](auto &picker,
                                                                                    const auto &pickerhitbox,
                                                                                    const auto &pickerPos) {
        if (picker.hasKey) return;

        for (const auto &key : world.view<entt::tag<"key"_hs>>()) {
            if (engine::d2::overlapped<engine::d2::WITH_EDGE>(
                    pickerhitbox, pickerPos, world.get<engine::d2::HitboxFloat>(key), world.get<engine::d3::Position>(key))) {
                picker.hasKey = true;
                world.destroy(key);
            }
        }
    });
}
