#include <spdlog/spdlog.h>
#include <sstream>

#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Event/Event.hpp>
#include <Engine/audio/AudioManager.hpp>
#include <Engine/Settings.hpp>
#include <Engine/component/Color.hpp>
#include <Engine/component/Texture.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Core.hpp>

#include "GameLogic.hpp"
#include "screen/MainMenu.hpp"
#include "ThePURGE.hpp"
#include "factory/EntityFactory.hpp"
#include "factory/SpellFactory.hpp"
#include "factory/ParticuleFactory.hpp"

#include "models/ClassDatabase.hpp"

using namespace std::chrono_literals;

game::GameLogic::GameLogic(ThePURGE &game) :
    m_game{game}, m_nextFloorSeed(static_cast<std::uint32_t>(std::time(nullptr)))
{
    sinkMovement.connect<&GameLogic::move>(*this);

    sinkOnGameStarted.connect<&GameLogic::on_game_started>(*this);

    sinkOnPlayerBuyClass.connect<&GameLogic::apply_class_to_player>(*this);
    sinkOnPlayerBuyClass.connect<&GameLogic::on_class_bought>(*this);
    sinkOnPlayerLevelUp.connect<&GameLogic::on_player_level_up>(*this);

    sinkGameUpdated.connect<&GameLogic::ai_pursue>(*this);
    sinkGameUpdated.connect<&GameLogic::cooldown>(*this);
    sinkGameUpdated.connect<&GameLogic::effect>(*this);
    sinkGameUpdated.connect<&GameLogic::enemies_try_attack>(*this);
    sinkGameUpdated.connect<&GameLogic::update_lifetime>(*this);
    sinkGameUpdated.connect<&GameLogic::update_particule>(*this);
    sinkGameUpdated.connect<&GameLogic::check_collision>(*this);
    sinkGameUpdated.connect<&GameLogic::exit_door_interraction>(*this);
    sinkGameUpdated.connect<&GameLogic::player_anim_update>(*this);

    sinkCastSpell.connect<&GameLogic::cast_attack>(*this);

    sinkGetKilled.connect<&GameLogic::entity_killed>(*this);
    sinkOnFloorChange.connect<&GameLogic::goToTheNextFloor>(*this);
}

auto game::GameLogic::move([[maybe_unused]] entt::registry &world, entt::entity &player, const engine::d2::Acceleration &accel)
    -> void
{
    world.get<engine::d2::Acceleration>(player) = accel;
}

auto game::GameLogic::on_game_started(entt::registry &world) -> void
{
    static auto holder = engine::Core::Holder{};

    holder.instance->getAudioManager()
        .getSound(holder.instance->settings().data_folder + "sounds/entrance_gong.wav")
        ->setVolume(0.2f)
        .play();
    m_game.getMusic()->play();

    m_game.player = EntityFactory::create<EntityFactory::PLAYER>(world, {}, {});
    apply_class_to_player(world, m_game.player, classes::getStarterClass(m_game.getClassDatabase()));

    // default camera value to see the generated terrain properly
    m_game.getCamera().setCenter(glm::vec2(13, 22));
    m_game.getCamera().setViewportSize(glm::vec2(25, 17));

    onFloorChange.publish(world);
}

auto game::GameLogic::apply_class_to_player(entt::registry &world, entt::entity player, const Class &newClass) -> void
{
    static auto holder = engine::Core::Holder{};

    world.get<AttackDamage>(player).damage = newClass.damage;

    auto &health = world.get<Health>(player);

    health.current += newClass.maxHealth - health.max;
    health.max = newClass.maxHealth;
    world.get<Classes>(player).ids.push_back(newClass.id);


    // TODO: actual spell selection ?

    for (const auto &spell : newClass.spells)
        for (auto &slot : world.get<SpellSlots>(player).spells) {
            if (slot.has_value()) continue;

            slot = Spell::create(spell);
            break;
        }

    auto &sp = world.replace<engine::Spritesheet>(
        player, engine::Spritesheet::from_json(holder.instance->settings().data_folder + newClass.assetGraphPath));

    // Doesn't really matter, will be overridden by correct one soon enough. Prevent segfault of accessing inexistant "default" animation
    sp.current_animation = "hold_right"; 
    
    engine::DrawableFactory::fix_texture(world, player, holder.instance->settings().data_folder + sp.file);


    { // Logging
        std::stringstream spellsId;
        for (const auto &spell : newClass.spells) spellsId << spell << ", ";

        std::stringstream childrens;
        for (const auto &child : newClass.childrenClass) childrens << m_game.getClassDatabase().at(child).name << ", ";

        spdlog::info(
            "Applied class '{}' to player. Stats are now : \n"
            "\tDamage : {:.3}\n"
            "\tMax health : {:.3}\n"
            "\tAdded spells {}\n"
            "\tNew available classes : {}",
            newClass.name,
            newClass.damage,
            newClass.maxHealth,
            spellsId.str(),
            childrens.str());
    }
}

auto game::GameLogic::on_class_bought(entt::registry &world, entt::entity player, const Class &) -> void
{
    world.get<SkillPoint>(player).count--;
}

auto game::GameLogic::on_player_level_up(entt::registry &world, entt::entity player) -> void
{
    world.get<SkillPoint>(player).count++;
}

auto game::GameLogic::ai_pursue(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
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

        out = {diff.x, diff.y};

        return true;
    };

    for (auto &i : world.view<entt::tag<"enemy"_hs>, engine::d3::Position, engine::d2::Velocity, game::ViewRange>()) {
        auto &vel = world.get<engine::d2::Velocity>(i);
        if (pursue(i, m_game.player, vel)) {

            if (world.has<engine::Spritesheet>(i)) {
                auto &sp = world.get<engine::Spritesheet>(i);
                sp.current_frame = sp.current_animation == "chase" ? sp.current_frame : 0;
                sp.current_animation = "chase";
            }

        } else {

            world.replace<engine::d2::Velocity>(i, (std::rand() & 1) ? -0.05 : 0.05, (std::rand() & 1) ? -0.05 : 0.05);

            if (world.has<engine::Spritesheet>(i)) {
                auto &sp = world.get<engine::Spritesheet>(i);
                sp.current_frame = sp.current_animation == "default" ? sp.current_frame : 0;
                sp.current_animation = "default";
            }

        }
    }
}

auto game::GameLogic::cooldown(entt::registry &world, const engine::TimeElapsed &dt) -> void
{
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(dt.elapsed).count();

    world.view<game::SpellSlots>().each([&](SpellSlots &slots) {
        for (auto &spell : slots.spells) {
            if (!spell.has_value()) continue;
            auto &cd = spell.value().cd;
            if (!cd.is_in_cooldown) continue;

            if (std::chrono::milliseconds{elapsed} < cd.remaining_cooldown) {
                cd.remaining_cooldown -= std::chrono::milliseconds{elapsed};
            } else {
                cd.remaining_cooldown = 0ms;
                cd.is_in_cooldown = false;
                spdlog::warn("attack is up !");
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

    for (auto &spell : world.view<entt::tag<"spell"_hs>>()) {
        const auto &spell_pos = world.get<engine::d3::Position>(spell);
        const auto &spell_box = world.get<engine::d2::HitboxFloat>(spell);

        for (auto &wall : world.view<entt::tag<"wall"_hs>>()) {
            const auto &wall_pos = world.get<engine::d3::Position>(wall);
            const auto &wall_box = world.get<engine::d2::HitboxSolid>(wall);

            if (engine::d2::overlapped<engine::d2::WITH_EDGE>(spell_box, spell_pos, wall_box, wall_pos)) {
                world.destroy(spell);
                break;
            }
        }
    }

    const auto apply_damage = [this, &world](auto &entity, auto &spell, auto &spell_hitbox, auto &spell_pos, auto &source) {
        auto &entity_pos = world.get<engine::d3::Position>(entity);
        auto &entity_hitbox = world.get<engine::d2::HitboxSolid>(entity);

        if (engine::d2::overlapped<engine::d2::WITHOUT_EDGE>(entity_hitbox, entity_pos, spell_hitbox, spell_pos)) {
            auto &entity_health = world.get<Health>(entity);
            auto &spell_damage = world.get<AttackDamage>(spell);

            // todo : publish player_took_damage instead
            entity_health.current -= spell_damage.damage;
            spdlog::warn("player took damage");

            if (world.has<entt::tag<"player"_hs>>(entity)) {
                holder.instance->setScreenshake(true, 300ms);
                ParticuleFactory::create<Particule::HITMARKER>(
                    world, {(spell_pos.x + entity_pos.x) / 2.0, (entity_pos.y + spell_pos.y) / 2.0});
            }

            holder.instance->getAudioManager()
                .getSound(holder.instance->settings().data_folder + "sounds/fire_hit.wav")
                ->play();
            world.destroy(spell);
            if (entity_health.current <= 0.0f) { playerKilled.publish(world, entity, source); }
        }
    };

    for (auto &spell : world.view<entt::tag<"spell"_hs>>()) {
        const auto &source = world.get<engine::Source>(spell).source;
        if (!world.valid(source)) return;

        auto &spell_pos = world.get<engine::d3::Position>(spell);
        const auto &hitbox = world.get<engine::d2::HitboxFloat>(spell);

        if (world.has<entt::tag<"player"_hs>>(source)) {
            for (auto &enemy : world.view<entt::tag<"enemy"_hs>>()) {
                apply_damage(enemy, spell, hitbox, spell_pos, source);
            }
        } else {
            for (auto &player : world.view<entt::tag<"player"_hs>>()) {
                apply_damage(player, spell, hitbox, spell_pos, source);
            }
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

// note : this should be in Core
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

auto game::GameLogic::update_particule(entt::registry &world, const engine::TimeElapsed &dt) -> void
{
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(dt.elapsed).count();
    for (const auto &i : world.view<Particule>()) {
        switch (world.get<Particule>(i).id) {
        case Particule::HITMARKER: {
            auto &color = world.get<engine::Color>(i);
            const auto r = engine::Color::r(color);
            const auto g = std::clamp(engine::Color::g(color) + 0.0001f * static_cast<float>(elapsed), 0.0f, 1.0f);
            const auto b = std::clamp(engine::Color::b(color) + 0.0001f * static_cast<float>(elapsed), 0.0f, 1.0f);
            engine::DrawableFactory::fix_color(world, i, {r, g, b});

            auto &vel = world.get<engine::d2::Velocity>(i);
            vel.x += ((std::rand() & 1) ? -1 : 1) * 0.005 * static_cast<double>(elapsed);
            vel.y += ((std::rand() & 1) ? -1 : 1) * 0.005 * static_cast<double>(elapsed);
        } break;
        default: break;
        }
    }
}

auto game::GameLogic::exit_door_interraction(entt::registry &world, const engine::TimeElapsed &) -> void
{
    const auto &door = world.view<entt::tag<"exit_door"_hs>>()[0];
    const auto &doorPosition = world.get<engine::d3::Position>(door);

    const auto doorUsageSystem = [&](const KeyPicker &keypicker, const engine::d3::Position &playerPos) {
        if (!keypicker.hasKey) return;

        if (engine::d3::distance(doorPosition, playerPos) < kDoorInteractionRange) onFloorChange.publish(world);
    };

    world.view<KeyPicker, engine::d3::Position>().each(doorUsageSystem);
}

auto game::GameLogic::player_anim_update(entt::registry &world, const engine::TimeElapsed &) -> void
{
    const auto &vel = world.get<engine::d2::Velocity>(m_game.player);
    const auto &facing = world.get<Facing>(m_game.player);
    auto &sp = world.get<engine::Spritesheet>(m_game.player);

    bool isFacingLeft;
    if (vel.x < 0)
        isFacingLeft = true;
    else if (vel.x > 0)
        isFacingLeft = false;
    else if (facing.dir.x < 0)
        isFacingLeft = true;
    else
        isFacingLeft = false;

    std::string anim;

    auto isMoving = vel.x != 0 || vel.y != 0;

    if (isMoving)
        if (isFacingLeft)
            anim = "run_left";
        else
            anim = "run_right";
    else
        if (isFacingLeft)
            anim = "hold_left";
        else
            anim = "hold_right";

    if (sp.current_animation != anim) {
        sp.current_animation = anim;
        sp.current_frame = 0;
    }
}

auto game::GameLogic::entity_killed(entt::registry &world, entt::entity killed, entt::entity killer) -> void
{
    static engine::Core::Holder holder{};

    if (world.has<entt::tag<"player"_hs>>(killed)) {
        holder.instance->getAudioManager()
            .getSound(holder.instance->settings().data_folder + "sounds/player_death.wav")
            ->play();

        m_game.setState(ThePURGE::State::GAME_OVER);
    } else if (world.has<entt::tag<"enemy"_hs>>(killed)) {
        spdlog::warn("!! entity killed : dropping xp !!");

        // TODO: actual random utilities
        bool lazyDevCoinflip = static_cast<std::uint32_t>(killed) % 2;
        holder.instance->getAudioManager()
            .getSound(
                lazyDevCoinflip ? holder.instance->settings().data_folder + "sounds/death_01.wav"
                                : holder.instance->settings().data_folder + "sounds/death_02.wav")
            ->play();

        // todo : move xp dropped as component or something
        if (world.has<entt::tag<"boss"_hs>>(killed))
            addXp(world, killer, 5);
        else
            addXp(world, killer, 1);

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
    if (!spell.cd.is_in_cooldown) {
        SpellFactory::create(spell.id, world, caster, glm::normalize(direction));
        spell.cd.remaining_cooldown = spell.cd.cooldown;
        spell.cd.is_in_cooldown = true;
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

auto game::GameLogic::addXp(entt::registry &world, entt::entity player, std::uint32_t xp) -> void
{
    auto &level = world.get<Level>(player);

    level.current_xp += xp;

    while (level.current_xp >= level.xp_require) {
        level.current_xp -= level.xp_require;
        level.xp_require = static_cast<std::uint32_t>(std::ceil(level.xp_require * 1.2));
        level.current_level++;

        onPlayerLevelUp.publish(world, player);
    }
}
