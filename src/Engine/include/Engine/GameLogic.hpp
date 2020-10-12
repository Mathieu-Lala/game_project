#pragma once

#include <entt/entt.hpp>

#include <Engine/component/Acceleration.hpp>
#include <Engine/Game.hpp>

#include "component/ViewRange.hpp"
#include "component/AttackRange.hpp"
#include "component/AttackDamage.hpp"
#include "component/AttackCooldown.hpp"
#include "component/Health.hpp"

namespace engine {

    class GameLogic {
    public:
        auto move([[maybe_unused]] entt::registry &world, entt::entity &player, const engine::d2::Acceleration &accel) -> void
        { 
            world.get<engine::d2::Acceleration>(player) = accel;
        };

        auto collision(entt::registry &world, entt::entity &player, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
        {
            world.view<entt::tag<"enemy"_hs>, engine::d2::Position, engine::d2::Velocity, game::ViewRange>().each(
                [&](auto &, auto &pos, auto &vel, auto &view_range) {
                    const auto player_pos = world.get<engine::d2::Position>(player);
                    const glm::vec2 diff = {player_pos.x - pos.x, player_pos.y - pos.y};

                    // if the enemy is close enough
                    if (glm::length(diff) <= view_range.range) {
                        vel = {diff.x, diff.y};
                    } else {
                        vel = {0, 0};
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
            world
                .view<entt::tag<"enemy"_hs>, engine::d2::Position, game::AttackRange, game::AttackCooldown, game::AttackDamage>()
                .each([&](auto &, auto &pos, auto &attack_range, auto &attack_cooldown, auto &attack_damage) {
                    const auto player_pos = world.get<engine::d2::Position>(player);
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
        }

        auto castSpell() -> void
        { 
            std::cout << "A spell have been casted !" << std::endl;
        }
    };
}
