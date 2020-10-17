#include <spdlog/spdlog.h>

#include <Engine/helpers/DrawableFactory.hpp>

#include <Engine/Event/Event.hpp>
#include <Engine/Core.hpp>

#include "GameLogic.hpp"
#include "ThePURGE.hpp"

game::GameLogic::GameLogic(ThePurge &game) : m_game{game}
{
    sinkMovement.connect<&GameLogic::move>(*this);

    sinkGameUpdated.connect<&GameLogic::ai_pursue>(*this);
    sinkGameUpdated.connect<&GameLogic::cooldown>(*this);
    sinkGameUpdated.connect<&GameLogic::effect>(*this);
    sinkGameUpdated.connect<&GameLogic::enemies_try_attack>(*this);
    sinkGameUpdated.connect<&GameLogic::update_lifetime>(*this);
    sinkGameUpdated.connect<&GameLogic::check_collision>(*this);

    sinkCastSpell.connect<&GameLogic::cast_attack>(*this);

    sinkGetKilled.connect<&GameLogic::entity_killed>(*this);
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

auto game::GameLogic::effect(entt::registry &world, const engine::TimeElapsed &dt) -> void
{
    auto player_health = world.get<game::Health>(m_game.player);
    
    world.view<game::Effect>().each([&](auto &effect) {
        if (!effect.is_in_effect) return;
        if (dt.elapsed < effect.remaining_time_effect) {
            effect.remaining_time_effect -= std::chrono::duration_cast<std::chrono::milliseconds>(dt.elapsed);
            if (effect.effect_name == "stun") spdlog::warn('stun');
            if (effect.effect_name == "bleed")
                player_health.current -= 0.01f /* (1 * (dt * 0.001)) true calcul but didn't found how do this calcul each sec to do it yet so TODO*/;
        } else {
            effect.is_in_effect = false;
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
    const auto apply_damage = [this, &world](auto &entity, auto &spell, auto &spell_hitbox, auto &spell_pos, auto &source){
        auto &entity_pos = world.get<engine::d3::Position>(entity);
        auto &entity_hitbox = world.get<engine::d2::HitboxSolid>(entity);

        if (engine::d2::overlapped(entity_hitbox, entity_pos, spell_hitbox, spell_pos)) {
            auto &entity_health = world.get<Health>(entity);
            auto &spell_damage = world.get<AttackDamage>(spell);

            // todo : publish player_took_damage instead
            entity_health.current -= spell_damage.damage;
            spdlog::warn("player took damage");

            world.destroy(spell);
            if (entity_health.current <= 0.0f) {
                playerKilled.publish(world, entity, source);
            }

        }
    };

    for (auto &spell : world.view<entt::tag<"spell"_hs>>()) {
        auto &spell_pos = world.get<engine::d3::Position>(spell);
        auto &spell_hitbox = world.get<engine::d2::HitboxFloat>(spell);
        const auto source = world.get<engine::Source>(spell).source;

        if (world.has<entt::tag<"player"_hs>>(source)) {
            for (auto &enemy : world.view<entt::tag<"enemy"_hs>>()) {
                apply_damage(enemy, spell, spell_hitbox, spell_pos, source);
            }
        } else {
            for (auto &player : world.view<entt::tag<"player"_hs>>()) {
                apply_damage(player, spell, spell_hitbox, spell_pos, source);
            }
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

auto game::GameLogic::entity_killed(entt::registry &world, entt::entity killed, entt::entity killer) -> void
{
    if (world.has<entt::tag<"player"_hs>>(killed)) {
        // note : may create segfault // assert fail
        m_game.setState(ThePurge::GAME_OVER);
    } else {
        spdlog::warn("!! entity killed : dropping xp !!");
        world.destroy(killed);

        // todo : send signal instead
        auto &level = world.get<Level>(killer);
        level.current_xp += 1;
        if (level.current_xp >= level.xp_require) {
            level.current_level++;
            level.current_xp = 0;
        }
    }
}

// todo : normalize direction
auto game::GameLogic::cast_attack(entt::registry &world, entt::entity entity, const glm::dvec2 &direction) -> void
{
    // todo : apply AttackDamage
    // todo : switch attack depending of entity type

    auto &attack_cooldown = world.get<AttackCooldown>(entity);
    auto &attack_damage = world.get<AttackDamage>(entity);
    auto &enemy_pos = world.get<engine::d3::Position>(entity);

    if (attack_cooldown.is_in_cooldown)
        return;

    attack_cooldown.is_in_cooldown = true;
    attack_cooldown.remaining_cooldown = attack_cooldown.cooldown;

    auto color = world.has<entt::tag<"enemy"_hs>>(entity) ? glm::vec3{0, 1, 0} : glm::vec3{1, 1, 0};

    const auto spell = world.create();
    world.emplace<entt::tag<"spell"_hs>>(spell);
    world.emplace<Lifetime>(spell, 600ms);
    world.emplace<AttackDamage>(spell, attack_damage.damage);
    world.emplace<engine::Drawable>(spell, engine::DrawableFactory::rectangle(std::move(color))).shader = &m_game.shader;
    world.emplace<engine::d3::Position>(spell, enemy_pos.x + direction.x / 2.0, enemy_pos.y + direction.y / 2.0, -1.0);
    world.emplace<engine::d2::Scale>(spell, 0.7, 0.7);
    world.emplace<engine::d2::HitboxFloat>(spell, 0.7, 0.7);
    world.emplace<engine::Source>(spell, entity);
}
