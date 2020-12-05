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

auto game::GameLogic::slots_apply_classes(entt::registry &world, entt::entity player, const Class &newClass) -> void
{
    static auto holder = engine::Core::Holder{};

    world.get<Speed>(player).speed = newClass.speed;
    world.get<engine::d2::HitboxSolid>(player) = newClass.hitbox;

    auto &health = world.get<Health>(player);

    health.current += newClass.health;
    health.max += newClass.health;
    world.get<Classes>(player).ids.push_back(newClass.name);

    // TODO: actual spell selection ?

    for (const auto &spell : newClass.spells)
        for (auto &slot : world.get<SpellSlots>(player).spells) {
            if (slot.has_value()) continue;
            slot = m_game.dbSpells().instantiate(spell);
            break;
        }

    world.replace<engine::Spritesheet>(
        player, engine::Spritesheet::from_json(holder.instance->settings().data_folder + newClass.assetGraphPath));

    engine::DrawableFactory::fix_spritesheet(world, player, "idle_right");

    world.get<SkillPoint>(player).count -= newClass.cost;
}

auto game::GameLogic::slots_level_up(entt::registry &world, entt::entity entity) -> void
{
    world.get<SkillPoint>(entity).count++;

    auto &level = world.get<Level>(entity);
    level.current_xp -= level.xp_require;
    level.xp_require = static_cast<std::uint32_t>(std::ceil(level.xp_require * 1.2));
    level.current_level++;

    auto pos = world.get<engine::d3::Position>(entity);
    ParticuleFactory::create<Particule::POSITIVE>(world, {pos.x, pos.y}, {255, 255, 0});
}

auto game::GameLogic::addXp(entt::registry &world, entt::entity player, std::uint32_t xp) -> void
{
    auto &level = world.get<Level>(player);

    level.current_xp += xp;

    while (level.current_xp >= level.xp_require) { onPlayerLevelUp.publish(world, player); }
}

auto game::GameLogic::slots_update_effect(entt::registry &world, const engine::TimeElapsed &dt) -> void
{
    auto holder = engine::Core::Holder{};

    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(dt.elapsed).count();

    world.view<entt::tag<"effect"_hs>, engine::Cooldown>().each([&elapsed](auto &, auto &cd) {
        if (!cd.is_in_cooldown) return;

        if (std::chrono::milliseconds{elapsed} <= cd.remaining_cooldown) {
            cd.remaining_cooldown -= std::chrono::milliseconds{elapsed};
        } else {
            cd.remaining_cooldown = 0ms;
            cd.is_in_cooldown = false;
        }
    });

    for (const auto &effect : world.view<entt::tag<"effect"_hs>>()) {
        auto &cd = world.get<engine::Cooldown>(effect);
        if (cd.is_in_cooldown) continue;
        cd.is_in_cooldown = true;
        cd.remaining_cooldown = cd.cooldown;

        const auto &receiver = world.get<engine::Source>(effect).source;
        const auto &sender = world.get<engine::SourceBis>(effect).source;
        if (!world.valid(receiver)) {
            world.destroy(effect);
            continue;
        }

        switch (world.get<Effect::Type>(effect)) {
        case Effect::Type::DOT: {
            auto &receiver_hp = world.get<game::Health>(receiver);

            // note : duplicated code !!! send a signal instead

            receiver_hp.current -= world.get<AttackDamage>(effect).damage;
            const auto is_player = world.has<entt::tag<"player"_hs>>(receiver);

            if (is_player) { holder.instance->setScreenshake(true, 350ms); }

            const auto &entity_pos = world.get<engine::d3::Position>(receiver);
            ParticuleFactory::create<Particule::HITMARKER>(
                world, {entity_pos.x, entity_pos.y}, is_player ? glm::vec3{255, 0, 0} : glm::vec3{0, 0, 0});

            holder.instance->getAudioManager()
                .getSound(holder.instance->settings().data_folder + "sounds/fire_hit.wav")
                ->play();

            if (receiver_hp.current <= 0.0f) {
                onEntityKilled.publish(world, receiver, sender);
                world.destroy(effect);
            }

        } break;
        case Effect::Type::DASH: {
            const auto &initial_speed = world.get<engine::Copy<Speed>>(receiver).data;
            world.replace<Speed>(receiver, initial_speed);
            world.remove_if_exists<engine::Copy<Speed>>(receiver);

            const auto &initial_color = world.get<engine::Copy<engine::Color>>(receiver).data;
            engine::DrawableFactory::fix_color(
                world,
                receiver,
                {engine::Color::r(initial_color), engine::Color::g(initial_color), engine::Color::b(initial_color)});
            world.remove_if_exists<engine::Copy<engine::Color>>(receiver);

        } break;
        default: break;
        }
    }
}

// note : not really on damage taken but rather, on collide with spell
auto game::GameLogic::slots_damage_taken(entt::registry &world, entt::entity receiver, entt::entity sender, entt::entity spell)
    -> void
{
    auto holder = engine::Core::Holder{};

    const auto effects = [&](const auto &ref) {
        struct sPsE {
            std::string_view tag;
            const Effect *effect;
        };
        std::vector<sPsE> out;
        std::transform(ref.begin(), ref.end(), std::back_inserter(out), [&](const auto &id) {
            return sPsE{id, &m_game.dbEffects().db.at(id)};
        });
        return out;
    }(world.get<SpellEffect>(spell).ref);

    for (auto &[new_tag, i] : effects) {
        // check if already instanciated & update the effect

        bool exist = false;
        world.view<entt::tag<"effect"_hs>, engine::Source, std::string>().each(
            [&exist, &new_tag, &receiver](auto &, const auto &source, const auto &tag) {
                exist |= tag == new_tag && source.source == receiver;
            });

        if (exist) {
            // spdlog::info("skipping effect already exist");

            // todo refresh cd

        } else {
            auto new_effect = world.create();
            world.emplace<entt::tag<"effect"_hs>>(new_effect);
            world.emplace<engine::Source>(new_effect, receiver);
            world.emplace<engine::SourceBis>(new_effect, sender);
            world.emplace<engine::Lifetime>(new_effect, i->lifetime);
            world.emplace<engine::Cooldown>(new_effect, true, i->cooldown, i->cooldown);
            world.emplace<std::string>(new_effect, new_tag);

            world.emplace<Effect::Type>(new_effect, i->type);

            if (i->type == Effect::DOT) {
                world.emplace<AttackDamage>(new_effect, i->damage);
            } else if (i->type == Effect::DASH) {
                spdlog::info("seting velocity");

                if (!world.has<engine::Copy<Speed>>(receiver)) {
                    const auto &current_speed = world.get<Speed>(receiver);
                    world.emplace<engine::Copy<Speed>>(receiver, current_speed);
                    world.replace<Speed>(receiver, current_speed.speed / i->strength);

                    const auto &current_color = world.get<engine::Color>(receiver);
                    world.emplace<engine::Copy<engine::Color>>(receiver, current_color);
                    engine::DrawableFactory::fix_color(world, receiver, {0, 0, 1});
                }
            }
        }
    }

    if (world.has<entt::tag<"projectile"_hs>>(spell)) {
        auto &entity_health = world.get<Health>(receiver);

        entity_health.current -= world.get<AttackDamage>(spell).damage;

        const auto is_player = world.has<entt::tag<"player"_hs>>(receiver);

        if (is_player) { holder.instance->setScreenshake(true, 350ms); }

        const auto &entity_pos = world.get<engine::d3::Position>(receiver);
        const auto &spell_pos = world.get<engine::d3::Position>(spell);
        ParticuleFactory::create<Particule::HITMARKER>(
            world,
            {(spell_pos.x + entity_pos.x) / 2.0, (entity_pos.y + spell_pos.y) / 2.0},
            is_player ? glm::vec3{255, 0, 0} : glm::vec3{0, 0, 0});

        holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/fire_hit.wav")->play();

        world.destroy(spell);

        if (entity_health.current <= 0.0f) { onEntityKilled.publish(world, receiver, sender); }
    }
}

auto game::GameLogic::slots_cast_spell(entt::registry &world, entt::entity caster, const glm::dvec2 &direction, Spell &spell)
    -> void
{
    if (!spell.cd.is_in_cooldown) {
        SpellFactory::create(world, caster, glm::normalize(direction), m_game.dbSpells().db.at(std::string{spell.id}));
        spell.cd.remaining_cooldown = spell.cd.cooldown;
        spell.cd.is_in_cooldown = true;
    }
}

auto game::GameLogic::slots_update_ai_attack(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
{
    for (auto enemy : world.view<entt::tag<"enemy"_hs>, engine::d3::Position, AttackRange>()) {
        // TODO: Add brain to AI. current strategy : spam every spell towards the player

        if (world.has<engine::Spritesheet>(enemy)
            && world.get<engine::Spritesheet>(enemy).current_animation.find("run") == std::string::npos)
            continue;

        for (auto &spell : world.get<SpellSlots>(enemy).spells) {
            if (!spell.has_value()) continue;

            const auto &selfPosition = world.get<engine::d3::Position>(enemy);
            const auto &targetPosition = world.get<engine::d3::Position>(m_game.player);

            const glm::vec2 diff = {targetPosition.x - selfPosition.x, targetPosition.y - selfPosition.y};

            auto &attack_range = world.get<AttackRange>(enemy);

            if (glm::length(diff) <= attack_range.range) {
                onSpellCast.publish(world, enemy, {diff.x, diff.y}, spell.value());
            }
        }
    }
}

auto game::GameLogic::slots_kill_entity(entt::registry &world, entt::entity killed, entt::entity killer) -> void
{
    static auto holder = engine::Core::Holder{};

    engine::DrawableFactory::fix_spritesheet(world, killed, "death");
    const auto &animation = world.get<engine::Spritesheet>(killed).animations["death"];

    world.emplace<engine::Lifetime>(killed, std::chrono::milliseconds(animation.frames.size() * animation.cooldown));
    world.get<engine::d2::Velocity>(killed) = {0.0, 0.0};

    world.remove<engine::d2::HitboxSolid>(killed);

    if (world.has<entt::tag<"player"_hs>>(killed)) {
        holder.instance->getAudioManager()
            .getSound(holder.instance->settings().data_folder + "sounds/player_death.wav")
            ->play();

        m_game.setMenu(std::make_unique<menu::GameOver>());

    } else if (world.has<entt::tag<"enemy"_hs>>(killed)) {
        // TODO: actual random utilities
        bool lazyDevCoinflip = static_cast<std::uint32_t>(killed) % 2;
        holder.instance->getAudioManager()
            .getSound(
                lazyDevCoinflip ? holder.instance->settings().data_folder + "sounds/death_01.wav"
                                : holder.instance->settings().data_folder + "sounds/death_02.wav")
            ->play();

        if (world.has<entt::tag<"player"_hs>>(killer)) { addXp(world, killer, world.get<Experience>(killed).xp); }

        if (world.has<entt::tag<"boss"_hs>>(killed)) {
            auto pos = world.get<engine::d3::Position>(killed);
            EntityFactory::create<EntityFactory::KEY>(m_game, world, {pos.x, pos.y}, {1.0, 1.0});
            holder.instance->getAudioManager()
                .getSound(holder.instance->settings().data_folder + "sounds/boss_death.wav")
                ->play();
        }
    }
}
