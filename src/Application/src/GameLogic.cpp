#include <spdlog/spdlog.h>

#include <Engine/helpers/DrawableFactory.hpp>

#include "GameLogic.hpp"
#include "ThePURGE.hpp"

game::GameLogic::GameLogic(ThePurge &game) : m_game{game}
{
    sinkMovement.connect<&GameLogic::move>(*this);

    sinkGameUpdated.connect<&GameLogic::ai_pursue>(*this);
    sinkGameUpdated.connect<&GameLogic::cooldown>(*this);
    sinkGameUpdated.connect<&GameLogic::enemies_try_attack>(*this);
    sinkGameUpdated.connect<&GameLogic::update_lifetime>(*this);
    sinkGameUpdated.connect<&GameLogic::check_collision>(*this);

    sinkCastSpell.connect<&GameLogic::cast_attack>(*this);

    sinkGetKilled.connect<&GameLogic::player_killed>(*this);
}

auto game::GameLogic::move([[maybe_unused]] entt::registry &world, entt::entity &player, const engine::d2::Acceleration &accel) -> void
{
    world.get<engine::d2::Acceleration>(player) = accel;
}

auto game::GameLogic::ai_pursue(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
{
    world.view<entt::tag<"enemy"_hs>, engine::d3::Position, engine::d2::Velocity, game::ViewRange>().each(
        [&](auto &, auto &pos, auto &vel, auto &view_range) {
            const auto player_pos = world.get<engine::d3::Position>(m_game.player);
            const glm::vec2 diff = {player_pos.x - pos.x, player_pos.y - pos.y};

            // if the enemy is close enough
            if (glm::length(diff) <= view_range.range) {
                vel = {diff.x, diff.y};
            } else {
                vel = {0, 0};
            }
        });
}

auto game::GameLogic::cooldown(entt::registry &world, const engine::TimeElapsed &dt) -> void
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

auto game::GameLogic::enemies_try_attack(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
{
    for (auto enemy : world.view<entt::tag<"enemy"_hs>, engine::d3::Position, AttackRange, AttackCooldown, AttackDamage>()) {
        auto &attack_cooldown = world.get<AttackCooldown>(enemy);
        if (attack_cooldown.is_in_cooldown) continue;

        auto &pos = world.get<engine::d3::Position>(enemy);
        const auto player_pos = world.get<engine::d3::Position>(m_game.player);
        const glm::vec2 diff = {player_pos.x - pos.x, player_pos.y - pos.y};

        auto &attack_range = world.get<AttackRange>(enemy);

        // if the enemy is close enough
        if (glm::length(diff) <= attack_range.range) {
            castSpell.publish(world, enemy, {diff.x, diff.y});
        }
    }
}

auto game::GameLogic::check_collision(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
{
    auto &player_pos = world.get<engine::d3::Position>(m_game.player);
    auto &player_hitbox = world.get<engine::d2::Hitbox>(m_game.player);

    auto &player_health = world.get<game::Health>(m_game.player);

    for (auto &spell : world.view<entt::tag<"spell"_hs>, engine::d2::Hitbox, engine::d3::Position>()) {

        auto &spell_pos = world.get<engine::d3::Position>(spell);
        auto &spell_hitbox = world.get<engine::d2::Hitbox>(spell);

        if (engine::d2::Hitbox::overlapped(player_hitbox, player_pos, spell_hitbox, spell_pos)) {
            // todo : publish player_took_damage instead

            player_health.current -= 10.0f;//attack_damage.damage;
            spdlog::warn("player took damage");

            if (player_health.current <= 0.0f) {
                playerKilled.publish(world, m_game.player);
//                player_health.current = player_health.max;

                // todo : send signal reset game or something ..
            }

            break;
        }

    }
}

auto game::GameLogic::update_lifetime(entt::registry &world, const engine::TimeElapsed &dt) -> void
{
    for (auto &i : world.view<Lifetime>()) {
        auto &lifetime = world.get<Lifetime>(i);

        if (dt.elapsed < lifetime.remaining_lifetime) {
            lifetime.remaining_lifetime -= std::chrono::duration_cast<std::chrono::milliseconds>(dt.elapsed);
        } else {
            world.destroy(i);
        }
    }
}

auto game::GameLogic::player_killed(entt::registry &world, entt::entity player) -> void
{
    world.destroy(player);
    spdlog::warn("!! player is dead !!! the application will now crash :)");
}

// todo : normalize direction
auto game::GameLogic::cast_attack(entt::registry &world, entt::entity entity, const glm::dvec2 &direction) -> void
{
    // todo : apply AttackDamage
    // todo : switch attack depending of entity type

    auto &attack_cooldown = world.get<AttackCooldown>(entity);
    //auto &attack_damage = world.get<AttackDamage>(entity);
    auto &enemy_pos = world.get<engine::d3::Position>(entity);

    if (attack_cooldown.is_in_cooldown)
        return;

    attack_cooldown.is_in_cooldown = true;
    attack_cooldown.remaining_cooldown = attack_cooldown.cooldown;

    const auto spell = world.create();
    world.emplace<entt::tag<"spell"_hs>>(spell);
    world.emplace<Lifetime>(spell, 600ms);
    world.emplace<engine::Drawable>(spell, engine::DrawableFactory::rectangle({0, 1, 0})).shader = &m_game.shader;
    world.emplace<engine::d3::Position>(spell, enemy_pos.x + direction.x / 2.0, enemy_pos.y + direction.y / 2.0, -1);
    world.emplace<engine::d2::Scale>(spell, 0.7, 0.7);
    world.emplace<engine::d2::Hitbox>(spell, 0.7, 0.7);
}
