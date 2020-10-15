#pragma once

#include <iostream>

#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

#include <Engine/Event/Event.hpp>

#include "component/all.hpp"
using namespace std::chrono_literals;


namespace game {

class GameLogic {
public:
    auto move([[maybe_unused]] entt::registry &world, entt::entity &player, const engine::d2::Acceleration &accel) -> void
    {
        world.get<engine::d2::Acceleration>(player) = accel;
    };

    auto collision(entt::registry &world, entt::entity &player, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
    {
        world.view<entt::tag<"enemy"_hs>, engine::d3::Position, engine::d2::Velocity, game::ViewRange>().each(
            [&](auto &, auto &pos, auto &vel, auto &view_range) {
                const auto player_pos = world.get<engine::d3::Position>(player);
                const glm::vec2 diff = {player_pos.x - pos.x, player_pos.y - pos.y};

                // if the enemy is close enough
                if (glm::length(diff) <= view_range.range) {
                    vel = {diff.x, diff.y};
                } else {
                    vel = {0, 0};
                }
            });
    }

    // a recup avant le merge
    auto effect(entt::registry &world, [[maybe_unused]] entt::entity &player, const engine::TimeElapsed &dt) -> void
    {
        auto &player_health = world.get<game::Health>(player);

        world.view<game::Effect>().each([&](auto &effect) {
            if (!effect.is_in_effect) return;

            if (dt.elapsed < effect.remaining_time_effect) {
                effect.remaining_time_effect -= std::chrono::duration_cast<std::chrono::milliseconds>(dt.elapsed);
                if (effect.effect_name == "stun") spdlog::warn('stun');
                if (effect.effect_name == "bleed") player_health.current -= 0.1f;

            } else {
                effect.is_in_effect = false;
            }
        });
    }

    auto cooldown(entt::registry &world, [[maybe_unused]] entt::entity &player, const engine::TimeElapsed &dt) -> void
    {
        world.view<game::AttackCooldown>().each([&](auto &attack_cooldown) {
            if (!attack_cooldown.is_in_cooldown) return;

            if (dt.elapsed < attack_cooldown.remaining_cooldown) {
                attack_cooldown.remaining_cooldown -= std::chrono::duration_cast<std::chrono::milliseconds>(dt.elapsed);
            } else {
                attack_cooldown.is_in_cooldown = false;
                spdlog::warn("attack is up !");
            }
        });
    }

    auto attack(entt::registry &world, entt::entity &player, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
    {
        auto &player_health = world.get<game::Health>(player);

        world.view<entt::tag<"enemy"_hs>, engine::d3::Position, AttackRange, AttackCooldown, AttackDamage>().each(
            [&](auto &, auto &pos, auto &attack_range, auto &attack_cooldown, auto &attack_damage) {
                const auto player_pos = world.get<engine::d3::Position>(player);
                const glm::vec2 diff = {player_pos.x - pos.x, player_pos.y - pos.y};

                // if the enemy is close enough
                if (glm::length(diff) <= attack_range.range && !attack_cooldown.is_in_cooldown) {
                    attack_cooldown.is_in_cooldown = true;
                    attack_cooldown.remaining_cooldown = attack_cooldown.cooldown;

                    player_health.current -= attack_damage.damage;
                    spdlog::warn("player took damage");

                    if (player_health.current <= 0.0f) {
                        spdlog::warn("!! player is dead, reseting the game");
                        player_health.current = player_health.max;

                        // todo : send signal reset game or something ..
                    }
                }
            });
        world.view<entt::tag<"boss"_hs>, engine::d3::Position, AttackRange, AttackCooldown, AttackDamage, Effect>().each(
            [&](auto &, auto &pos, auto &attack_range, auto &attack_cooldown, auto &attack_damage, auto &effect) {
                const auto player_pos = world.get<engine::d3::Position>(player);
                const glm::vec2 diff = {player_pos.x - pos.x, player_pos.y - pos.y};

                // if the enemy is close enough
                if (glm::length(diff) <= attack_range.range && !attack_cooldown.is_in_cooldown) {
                    attack_cooldown.is_in_cooldown = true;
                    attack_cooldown.remaining_cooldown = attack_cooldown.cooldown;
                    effect.is_in_cooldown = true;
                    effect.is_in_effect = true;
                    effect.remaining_cooldown = effect.cooldown;
                    effect.remaining_time_effect = effect.time_effect;
                    
                    player_health.current -= attack_damage.damage;
                    spdlog::warn("player took damage");

                    if (player_health.current <= 0.0f) {
                        spdlog::warn("!! player is dead, reseting the game");
                        player_health.current = player_health.max;

                        // todo : send signal reset game or something ..
                    }
                }
            });
    }

    auto castSpell() -> void { std::cout << "A spell have been casted !" << std::endl; }
};
} // namespace game
