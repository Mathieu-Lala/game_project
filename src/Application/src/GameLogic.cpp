#include <spdlog/spdlog.h>

#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Event/Event.hpp>
#include <Engine/audio/AudioManager.hpp>
#include <Engine/Settings.hpp>
#include <Engine/Core.hpp>

#include "GameLogic.hpp"
#include "ThePURGE.hpp"
#include "factory/EntityFactory.hpp"
#include "factory/SpellFactory.hpp"

using namespace std::chrono_literals;

game::GameLogic::GameLogic(ThePurge &game) :
    m_game{game}, m_nextFloorSeed(static_cast<std::uint32_t>(std::time(nullptr)))
{
    sinkMovement.connect<&GameLogic::move>(*this);

    sinkGameUpdated.connect<&GameLogic::ai_pursue>(*this);
    sinkGameUpdated.connect<&GameLogic::cooldown>(*this);
    sinkGameUpdated.connect<&GameLogic::effect>(*this);
    sinkGameUpdated.connect<&GameLogic::enemies_try_attack>(*this);
    sinkGameUpdated.connect<&GameLogic::update_lifetime>(*this);
    sinkGameUpdated.connect<&GameLogic::check_collision>(*this);
    sinkGameUpdated.connect<&GameLogic::exit_door_interraction>(*this);

    sinkCastSpell.connect<&GameLogic::cast_attack>(*this);

    sinkGetKilled.connect<&GameLogic::entity_killed>(*this);
    sinkOnFloorChange.connect<&GameLogic::goToTheNextFloor>(*this);
}

auto game::GameLogic::move([[maybe_unused]] entt::registry &world, entt::entity &player, const engine::d2::Acceleration &accel)
    -> void
{
    world.get<engine::d2::Acceleration>(player) = accel;
}

auto game::GameLogic::ai_pursue(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
{
    for (auto &i : world.view<entt::tag<"enemy"_hs>, engine::d3::Position, engine::d2::Velocity, game::ViewRange>()) {
        auto &pos = world.get<engine::d3::Position>(i);
        //auto &vel = world.get<engine::d2::Velocity>(i);
        auto &view_range = world.get<ViewRange>(i);

        const auto player_pos = world.get<engine::d3::Position>(m_game.player);
        const glm::vec2 diff = {player_pos.x - pos.x, player_pos.y - pos.y};

        static bool chasing = false; // tmp : true if the boss is chasing the player

        // if the enemy is close enough
        if (glm::length(diff) <= view_range.range) {
            world.replace<engine::d2::Velocity>(i, diff.x, diff.y);

            if (world.has<entt::tag<"boss"_hs>>(i)) { // tmp
                if (chasing) continue;

                auto &sp = world.get<engine::Spritesheet>(i);
                sp.current_animation = "hold";
                sp.current_frame = 0;

                chasing = true;
            }

        } else {
            // todo : make the enemy move randomly
            world.replace<engine::d2::Velocity>(i, 0.0f, 0.0f);

            if (world.has<entt::tag<"boss"_hs>>(i)) { // tmp
                if (!chasing) continue;

                auto &sp = world.get<engine::Spritesheet>(i);
                sp.current_animation = "default";
                sp.current_frame = 0;

                chasing = false;
            }
        }
    }
}

auto game::GameLogic::cooldown(entt::registry &world, const engine::TimeElapsed &dt) -> void
{
    world.view<game::SpellSlots>().each([&](SpellSlots &slots) {
        for (auto &spell : slots.spells) {
            if (!spell.has_value()) continue;

            if (spell->current_cooldown > 0ms) {
                spell->current_cooldown -= std::chrono::duration_cast<std::chrono::milliseconds>(dt.elapsed);

                // it's <= to still print the message even if the cooldown arrive directly at 0
                if (spell->current_cooldown <= 0ms) {
                    spell->current_cooldown = 0ms;
                    spdlog::warn("attack is up !");
                }
            }
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
            if (effect.effect_name == "stun") spdlog::warn("stun");
            if (effect.effect_name == "bleed") {
                /* (1 * (dt * 0.001)) true calcul but didn't found how do this calcul each sec to do it yet so TODO*/
                player_health.current -= 0.01f;
            }
        } else {
            effect.is_in_effect = false;
        }
    });
}

auto game::GameLogic::enemies_try_attack(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
{
    for (auto enemy : world.view<entt::tag<"enemy"_hs>, engine::d3::Position, AttackRange>()) {
        // TODO: Add brain to AI. current strategy : spam every spell towards the player

        for (auto &spell : world.get<SpellSlots>(enemy).spells) {
            if (!spell.has_value()) continue;

            const auto &selfPosition = world.get<engine::d3::Position>(enemy);
            const auto &targetPosition = world.get<engine::d3::Position>(m_game.player);

            const glm::vec2 diff = {targetPosition.x - selfPosition.x, targetPosition.y - selfPosition.y};

            auto &attack_range = world.get<AttackRange>(enemy);

            if (glm::length(diff) <= attack_range.range) {
                castSpell.publish(world, enemy, {diff.x, diff.y}, spell.value());
            }
        }
    }
}

auto game::GameLogic::check_collision(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
{
    static engine::Core::Holder holder{};

    // todo : handle collision terrain and spell

    // todo : spell (fireball) should destroy on collide

    const auto apply_damage = [this, &world](auto &entity, auto &spell, auto &spell_hitbox, auto &spell_pos, auto &source) {
        auto &entity_pos = world.get<engine::d3::Position>(entity);
        auto &entity_hitbox = world.get<engine::d2::HitboxSolid>(entity);

        if (engine::d2::overlapped<engine::d2::WITHOUT_EDGE>(entity_hitbox, entity_pos, spell_hitbox, spell_pos)) {
            auto &entity_health = world.get<Health>(entity);
            auto &spell_damage = world.get<AttackDamage>(spell);

            // todo : publish player_took_damage instead
            entity_health.current -= spell_damage.damage;
            spdlog::warn("player took damage");

            holder.instance->setScreenshake(true, 300ms);

            holder.instance->getAudioManager()
                .getSound(holder.instance->settings().data_folder + "sounds/fire_hit.wav")
                ->play();
            world.destroy(spell);
            if (entity_health.current <= 0.0f) { playerKilled.publish(world, entity, source); }
        }
    };

    auto check_collision_spell = [&world, &apply_damage]<engine::d2::HitboxType T>(
                                     entt::entity spell, entt::entity source, const engine::d2::Hitbox<T> &hitbox) {
        auto &spell_pos = world.get<engine::d3::Position>(spell);

        if (!world.valid(source)) return;

        if (world.has<entt::tag<"player"_hs>>(source)) {
            for (auto &enemy : world.view<entt::tag<"enemy"_hs>>()) {
                apply_damage(enemy, spell, hitbox, spell_pos, source);
            }
        } else {
            for (auto &player : world.view<entt::tag<"player"_hs>>()) {
                apply_damage(player, spell, hitbox, spell_pos, source);
            }
        }
    };

    for (auto &spell : world.view<entt::tag<"spell"_hs>>()) {
        if (world.has<engine::d2::HitboxFloat>(spell)) {
            check_collision_spell(spell, world.get<engine::Source>(spell).source, world.get<engine::d2::HitboxFloat>(spell));
        } else if (world.has<engine::d2::HitboxSolid>(spell)) {
            check_collision_spell(spell, world.get<engine::Source>(spell).source, world.get<engine::d2::HitboxSolid>(spell));
        }
    }

    world.view<KeyPicker, engine::d2::HitboxSolid, engine::d3::Position>().each(
        [&](KeyPicker &keypicker, const engine::d2::HitboxSolid &pickerhitbox, const engine::d3::Position &pickerPos) {
            if (keypicker.hasKey) return;

            for (auto &key : world.view<entt::tag<"key"_hs>>()) {
                auto &keyHitbox = world.get<engine::d2::HitboxFloat>(key);
                auto &keyPos = world.get<engine::d3::Position>(key);

                if (engine::d2::overlapped<engine::d2::WITH_EDGE>(pickerhitbox, pickerPos, keyHitbox, keyPos)) {
                    keypicker.hasKey = true;

                    world.destroy(key);
                }
            }
        });
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

auto game::GameLogic::exit_door_interraction(entt::registry &world, const engine::TimeElapsed &) -> void
{
    const auto &door = world.view<entt::tag<"exit_door"_hs>>()[0];
    const auto &doorPosition = world.get<engine::d3::Position>(door);

    const auto doorUsageSystem = [&](const KeyPicker &keypicker, const engine::d3::Position &playerPos) {
        if (!keypicker.hasKey) return;

        // would be cool if : && playerPos.hasVisionOn(doorPosition);
        if (engine::d3::distance(doorPosition, playerPos) < kDoorInteractionRange) onFloorChange.publish(world);
    };

    world.view<KeyPicker, engine::d3::Position>().each(doorUsageSystem);
}

auto game::GameLogic::entity_killed(entt::registry &world, entt::entity killed, entt::entity killer) -> void
{
    static engine::Core::Holder holder{};

    if (world.has<entt::tag<"player"_hs>>(killed)) {
        holder.instance->getAudioManager()
            .getSound(holder.instance->settings().data_folder + "sounds/player_death.wav")
            ->play();

        m_game.setState(ThePurge::State::GAME_OVER);
    } else if (world.has<entt::tag<"enemy"_hs>>(killed)) {
        spdlog::warn("!! entity killed : dropping xp !!");

        // TODO: actual random utilities
        bool lazyDevCoinflip = static_cast<std::uint32_t>(killed) % 2;
        holder.instance->getAudioManager()
            .getSound(
                lazyDevCoinflip ? holder.instance->settings().data_folder + "sounds/death_01.wav"
                                : holder.instance->settings().data_folder + "sounds/death_02.wav")
            ->play();

        // todo : send signal instead
        auto &level = world.get<Level>(killer);
        // todo : move this as component or something
        level.current_xp += world.has<entt::tag<"boss"_hs>>(killed) ? 5u : 1u;
        if (level.current_xp >= level.xp_require) {
            level.current_level++;
            level.current_xp = 0;
        }

        if (world.has<entt::tag<"boss"_hs>>(killed)) {
            auto pos = world.get<engine::d3::Position>(killed);
            EntityFactory::create<EntityFactory::KEY>(world, {pos.x, pos.y}, {1.0, 1.0});
            holder.instance->getAudioManager()
                .getSound(holder.instance->settings().data_folder + "sounds/boss_death.wav")
                ->play();
        }
        world.destroy(killed);
    }
}

// todo : normalize direction
auto game::GameLogic::cast_attack(entt::registry &world, entt::entity caster, const glm::dvec2 &direction, Spell &spell)
    -> void
{
    if (spell.current_cooldown == 0ms) {
        SpellFactory::create(spell.id, world, caster, direction);
        spell.current_cooldown = spell.cooldown_duration;
    }
}

auto game::GameLogic::goToTheNextFloor(entt::registry &world) -> void
{
    world.view<entt::tag<"terrain"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<entt::tag<"enemy"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<entt::tag<"spell"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<entt::tag<"key"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<KeyPicker>().each([&](KeyPicker &kp) { kp.hasKey = false; });

    auto data = generateFloor(world, m_map_generation_params, m_nextFloorSeed);
    m_nextFloorSeed = data.nextFloorSeed;

    auto allPlayers = world.view<entt::tag<"player"_hs>>();

    for (auto &player : allPlayers) {
        auto &pos = world.get<engine::d3::Position>(player);

        pos.x = data.spawn.x + data.spawn.w * 0.5;
        pos.y = data.spawn.y + data.spawn.h * 0.5;
    }
}
