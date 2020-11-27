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
    sinkMovement.connect<&GameLogic::slots_move>(*this);
    // sinkJoystickMovement.connect<&GameLogic::joystickMove>(*this);

    sinkOnGameStarted.connect<&GameLogic::slots_game_start>(*this);

    sinkOnPlayerBuyClass.connect<&GameLogic::slots_apply_classes>(*this);
    sinkOnPlayerBuyClass.connect<&GameLogic::slots_purchase_classes>(*this);
    sinkOnPlayerLevelUp.connect<&GameLogic::slots_level_up>(*this);

    sinkGameUpdated.connect<&GameLogic::slots_update_player_movement>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_update_ai_movement>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_update_ai_attack>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_update_particle>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_update_cooldown>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_update_effect>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_check_collision>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_check_floor_change>(*this);

    sinkGameUpdated.connect<&GameLogic::player_anim_update>(*this);
    sinkGameUpdated.connect<&GameLogic::boss_anim_update>(*this);

    sinkAfterGameUpdated.connect<&GameLogic::slots_update_camera>(*this);
    sinkAfterGameUpdated.connect<&GameLogic::slots_update_player_sigh>(*this);

    sinkCastSpell.connect<&GameLogic::slots_cast_spell>(*this);

    sinkGetKilled.connect<&GameLogic::slots_kill_entity>(*this);
    sinkOnFloorChange.connect<&GameLogic::slots_change_floor>(*this);
}

auto game::GameLogic::slots_move([[maybe_unused]] entt::registry &world, entt::entity &player, const Direction &dir, const float spd, bool is_pressed)
    -> void
{
    switch (dir) {
    case Direction::UP: world.get<ControllerAxis>(player).movement.y = is_pressed ? 1.0f + spd : 0.0f; break;
    case Direction::DOWN: world.get<ControllerAxis>(player).movement.y = is_pressed ? -1.0f - spd : 0.0f; break;
    case Direction::RIGHT: world.get<ControllerAxis>(player).movement.x = is_pressed ? 1.0f + spd : 0.0f; break;
    case Direction::LEFT: world.get<ControllerAxis>(player).movement.x = is_pressed ? -1.0f - spd : 0.0f; break;
    default: break;
    }
}

// auto game::GameLogic::joystickMove(entt::registry &world, entt::entity &player, const engine::d2::Acceleration &accel) -> void
//{
//    world.get<engine::d2::Acceleration>(player) = accel;
//}

auto game::GameLogic::slots_game_start(entt::registry &world) -> void
{
    static auto holder = engine::Core::Holder{};

    holder.instance->getAudioManager()
        .getSound(holder.instance->settings().data_folder + "sounds/entrance_gong.wav")
        ->setVolume(0.2f)
        .play();
    m_game.getMusic()->play();

    m_game.player = EntityFactory::create<EntityFactory::PLAYER>(world, {}, {});
    onPlayerPurchase.publish(world, m_game.player, classes::getStarterClass(m_game.getClassDatabase()));

    auto aimingSight = EntityFactory::create<EntityFactory::ID::AIMING_SIGHT>(world, {}, {});

    glm::vec3 playerColor(1.f, 0.2f, 0.2f);
    engine::DrawableFactory::fix_color(world, aimingSight, std::move(playerColor));
    world.get<AimSight>(m_game.player).entity = aimingSight;

    // default camera value to see the generated terrain properly
    m_game.getCamera().setCenter(glm::vec2(13, 22));
    m_game.getCamera().setViewportSize(glm::vec2(25, 17));

    onFloorChange.publish(world);
}

auto game::GameLogic::slots_apply_classes(entt::registry &world, entt::entity player, const Class &newClass) -> void
{
    static auto holder = engine::Core::Holder{};

    world.get<AttackDamage>(player).damage = newClass.damage;
    world.get<Speed>(player).speed = newClass.speed;
    world.get<engine::d2::HitboxSolid>(player) = newClass.hitbox;

    auto &health = world.get<Health>(player);

    health.current += newClass.maxHealth - health.max;
    health.max += newClass.maxHealth;
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
        for (const auto &child : newClass.children) childrens << m_game.getClassDatabase().at(child).name << ", ";

        spdlog::info(
            "Applied class '{}' to player. Stats are now : \n"
            "\tDamage : {:.3}\n"
            "\tMax health : {:.3}\n"
            "\tAdded spells {}\n"
            "\tNew available classes : {}",
            newClass.name,
            newClass.damage,
            newClass.maxHealth,
            newClass.speed,
            spellsId.str(),
            childrens.str());
    }
}

auto game::GameLogic::slots_purchase_classes(entt::registry &world, entt::entity player, const Class &) -> void
{
    world.get<SkillPoint>(player).count--;
}

auto game::GameLogic::slots_level_up(entt::registry &world, entt::entity entity) -> void
{
    world.get<SkillPoint>(entity).count++;
}

auto game::GameLogic::slots_update_player_movement(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt)
    -> void
{
    constexpr float kSpeed = 10;

    auto player = m_game.player;

    auto &vel = world.get<engine::d2::Velocity>(player);
    const auto &axis = world.get<ControllerAxis>(player);

    vel.x = axis.movement.x * (kSpeed);
    vel.y = axis.movement.y * (kSpeed);
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

        auto result = glm::normalize(diff) * 7.f;
        out = {result.x, result.y};

        return true;
    };

    for (auto &i : world.view<entt::tag<"enemy"_hs>, engine::d3::Position, engine::d2::Velocity, game::ViewRange>()) {
        auto &vel = world.get<engine::d2::Velocity>(i);
        pursue(i, m_game.player, vel);
    }
}

auto game::GameLogic::slots_update_cooldown(entt::registry &world, const engine::TimeElapsed &dt) -> void
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

auto game::GameLogic::slots_update_effect(entt::registry &world, const engine::TimeElapsed &dt) -> void
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

auto game::GameLogic::slots_update_ai_attack(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
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
                onSpellCast.publish(world, enemy, {diff.x, diff.y}, spell.value());
            }
        }
    }
}

auto game::GameLogic::slots_check_collision(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
{
    static auto holder = engine::Core::Holder{};

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

            // todo : publish entity_took_damage instead
            entity_health.current -= spell_damage.damage;

            if (world.has<entt::tag<"player"_hs>>(entity)) {
                holder.instance->setScreenshake(true, 300ms);
                ParticuleFactory::create<Particule::HITMARKER>(
                    world, {(spell_pos.x + entity_pos.x) / 2.0, (entity_pos.y + spell_pos.y) / 2.0});
            }

            holder.instance->getAudioManager()
                .getSound(holder.instance->settings().data_folder + "sounds/fire_hit.wav")
                ->play();
            world.destroy(spell);
            if (entity_health.current <= 0.0f) { onEntityKilled.publish(world, entity, source); }
        }
    };

    for (auto &spell : world.view<entt::tag<"spell"_hs>>()) {
        const auto &source = world.get<engine::Source>(spell).source;
        if (!world.valid(source)) return;

        auto &spell_pos = world.get<engine::d3::Position>(spell);
        const auto &hitbox = world.get<engine::d2::HitboxFloat>(spell);

        if (world.has<entt::tag<"player"_hs>>(source)) {
            for (auto &enemy : world.view<entt::tag<"enemy"_hs>>()) {
                if (world.valid(spell)) apply_damage(enemy, spell, hitbox, spell_pos, source);
            }
        } else {
            for (auto &player : world.view<entt::tag<"player"_hs>>()) {
                if (world.valid(spell)) apply_damage(player, spell, hitbox, spell_pos, source);
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

auto game::GameLogic::slots_update_particle(
    [[maybe_unused]] entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt) -> void
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

auto game::GameLogic::slots_check_floor_change(entt::registry &world, const engine::TimeElapsed &) -> void
{
    world.view<KeyPicker, engine::d3::Position>().each(
        [door = world.view<entt::tag<"exit_door"_hs>>()[0], &world, this](const auto &picker, const auto &pos) {
            if (!picker.hasKey) return;

            constexpr auto kDoorInteractionRange = 2.0;
            if (engine::d3::distance(world.get<engine::d3::Position>(door), pos) < kDoorInteractionRange)
                onFloorChange.publish(world);
        });
}

auto game::GameLogic::slots_update_camera(entt::registry &world, const engine::TimeElapsed &) -> void
{
    static auto holder = engine::Core::Holder{};

    auto player = m_game.player;

    auto &pos = world.get<engine::d3::Position>(player);
    m_game.getCamera().setCenter({pos.x, pos.y});
    if (m_game.getCamera().isUpdated()) holder.instance->updateView(m_game.getCamera().getViewProjMatrix());
}

auto game::GameLogic::player_anim_update(entt::registry &world, const engine::TimeElapsed &) -> void
{
    const auto &vel = world.get<engine::d2::Velocity>(m_game.player);
    const auto &aiming = world.get<AimingDirection>(m_game.player).dir;
    auto &sp = world.get<engine::Spritesheet>(m_game.player);

    bool isFacingLeft;
    if (vel.x < 0)
        isFacingLeft = true;
    else if (vel.x > 0)
        isFacingLeft = false;
    else if (aiming.x < 0)
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
    else if (isFacingLeft)
        anim = "hold_left";
    else
        anim = "hold_right";

    if (sp.current_animation != anim) {
        sp.current_animation = anim;
        sp.current_frame = 0;
    }
}

auto game::GameLogic::slots_update_player_sigh(entt::registry &world, const engine::TimeElapsed &) -> void
{
    constexpr auto kSightMaxDistance = 1.5;
    constexpr auto kSightScaleMultiplier = 0.75;

    auto player = m_game.player;
    auto sight = world.get<AimSight>(player).entity;
    const auto &aimInput = world.get<ControllerAxis>(player).aiming;

    if (glm::length(aimInput)) {
        auto &aim = world.get<AimingDirection>(player).dir;
        auto newAim = glm::normalize(aimInput);

        aim.x = newAim.x;
        aim.y = newAim.y;
    }

    const auto &playerPos = world.get<engine::d3::Position>(player);
    auto &sightPos = world.get<engine::d3::Position>(sight);
    auto &sightScale = world.get<engine::d2::Scale>(sight);

    sightPos.x = playerPos.x + kSightMaxDistance * static_cast<double>(aimInput.x);
    sightPos.y = playerPos.y + kSightMaxDistance * static_cast<double>(aimInput.y);

    const auto scale = static_cast<double>(glm::length(aimInput));
    sightScale.x = scale * kSightScaleMultiplier;
    sightScale.y = scale * kSightScaleMultiplier;
}

auto game::GameLogic::boss_anim_update(entt::registry &world, const engine::TimeElapsed &) -> void
{
    // we keep it as static to be consister with previous frame on standstill
    static bool isFacingLeft = false;

    auto queryRes = world.view<entt::tag<"boss"_hs>>();

    if (queryRes.size() == 0) // boss is dead
        return;

    auto boss = queryRes.front();

    const auto &vel = world.get<engine::d2::Velocity>(boss);
    auto &sp = world.get<engine::Spritesheet>(boss);

    isFacingLeft = vel.x < 0 || (isFacingLeft && vel.x == 0);

    std::string anim;

    auto isMoving = vel.x != 0 || vel.y != 0;

    if (isMoving)
        if (isFacingLeft)
            anim = "run_left";
        else
            anim = "run_right";
    else if (isFacingLeft)
        anim = "hold_left";
    else
        anim = "hold_right";

    if (sp.current_animation != anim) {
        spdlog::info("Changing anim from {} to {}. Vel is {}, {}", sp.current_animation, anim, vel.x, vel.y);
        sp.current_animation = anim;
        sp.current_frame = 0;
    }
}

auto game::GameLogic::slots_kill_entity(entt::registry &world, entt::entity killed, entt::entity killer) -> void
{
    static auto holder = engine::Core::Holder{};

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

        if (world.has<entt::tag<"player"_hs>>(killer)) {
            // todo : move xp dropped as component or something
            if (world.has<entt::tag<"boss"_hs>>(killed))
                addXp(world, killer, 5);
            else
                addXp(world, killer, 1);
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

auto game::GameLogic::slots_cast_spell(entt::registry &world, entt::entity caster, const glm::dvec2 &direction, Spell &spell)
    -> void
{
    if (!spell.cd.is_in_cooldown) {
        SpellFactory::create(spell.id, world, caster, glm::normalize(direction));
        spell.cd.remaining_cooldown = spell.cd.cooldown;
        spell.cd.is_in_cooldown = true;
    }
}

auto game::GameLogic::slots_change_floor(entt::registry &world) -> void
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
