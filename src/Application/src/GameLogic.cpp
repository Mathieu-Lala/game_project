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
#include "ThePURGE.hpp"
#include "factory/EntityFactory.hpp"
#include "factory/SpellFactory.hpp"
#include "factory/ParticuleFactory.hpp"

#include "models/ClassDatabase.hpp"

#include "menu/UpgradePanel.hpp"
#include "menu/GameOver.hpp"

using namespace std::chrono_literals;

game::GameLogic::GameLogic(ThePURGE &game) :
    m_game{game}, m_nextFloorSeed(static_cast<std::uint32_t>(std::time(nullptr)))
{
    sinkMovement.connect<&GameLogic::slots_move>(*this);

    sinkOnGameStarted.connect<&GameLogic::slots_game_start>(*this);

    sinkOnPlayerBuyClass.connect<&GameLogic::slots_apply_classes>(*this);
    sinkOnPlayerBuyClass.connect<&GameLogic::slots_purchase_classes>(*this);
    sinkOnPlayerLevelUp.connect<&GameLogic::slots_level_up>(*this);

    sinkOnEvent.connect<&GameLogic::slots_on_event>(*this);

    sinkGameUpdated.connect<&GameLogic::slots_update_player_movement>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_update_ai_movement>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_update_ai_attack>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_update_particle>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_update_cooldown>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_update_effect>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_check_collision>(*this);
    sinkGameUpdated.connect<&GameLogic::slots_check_floor_change>(*this);

    sinkGameUpdated.connect<&GameLogic::slots_update_animation_spritesheet>(*this);

    sinkAfterGameUpdated.connect<&GameLogic::slots_update_camera>(*this);
    sinkAfterGameUpdated.connect<&GameLogic::slots_update_player_sigh>(*this);

    sinkCastSpell.connect<&GameLogic::slots_cast_spell>(*this);

    sinkGetKilled.connect<&GameLogic::slots_kill_entity>(*this);
    sinkOnFloorChange.connect<&GameLogic::slots_change_floor>(*this);
}

auto game::GameLogic::slots_move([[maybe_unused]] entt::registry &world, entt::entity &player, const Direction &dir, bool is_pressed)
    -> void
{
    auto spd = world.get<Speed>(player).speed;

    switch (dir) {
    case Direction::UP: world.get<ControllerAxis>(player).movement.y = is_pressed ? spd : 0.0f; break;
    case Direction::DOWN: world.get<ControllerAxis>(player).movement.y = is_pressed ? -spd : 0.0f; break;
    case Direction::RIGHT: world.get<ControllerAxis>(player).movement.x = is_pressed ? spd : 0.0f; break;
    case Direction::LEFT: world.get<ControllerAxis>(player).movement.x = is_pressed ? -spd : 0.0f; break;
    default: break;
    }
}

auto game::GameLogic::slots_game_start(entt::registry &world) -> void
{
    static auto holder = engine::Core::Holder{};

    holder.instance->getAudioManager()
        .getSound(holder.instance->settings().data_folder + "sounds/entrance_gong.wav")
        ->setVolume(0.2f)
        .play();
    m_game.getBackgroundMusic()->play();

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

    health.current += newClass.maxHealth;
    health.max += newClass.maxHealth;
    world.get<Classes>(player).ids.push_back(newClass.id);


    // TODO: actual spell selection ?

    for (const auto &spell : newClass.spells)
        for (auto &slot : world.get<SpellSlots>(player).spells) {
            if (slot.has_value()) continue;

            slot = Spell::create(spell);
            break;
        }

    world.replace<engine::Spritesheet>(
        player, engine::Spritesheet::from_json(holder.instance->settings().data_folder + newClass.assetGraphPath));

    // Doesn't really matter, will be overridden by correct one soon enough. Prevent segfault of accessing inexistant
    // "default" animation sp.current_animation = "idle_right";

    // engine::DrawableFactory::fix_texture(world, player, holder.instance->settings().data_folder + sp.file);
    engine::DrawableFactory::fix_spritesheet(world, player, "idle_right");


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

auto game::GameLogic::slots_on_event(entt::registry &world, const engine::Event &e) -> void
{
    static auto holder = engine::Core::Holder{};

    const auto spell_map = []<typename T>(T k) {
        struct SpellMap {
            int key;
            std::size_t id;
        };
        if constexpr (std::is_same<T, engine::Joystick::Buttons>::value) {
            const auto map = std::to_array<SpellMap>({{engine::Joystick::LS, 0}, {engine::Joystick::RS, 1}});
            return std::find_if(map.begin(), map.end(), [&k](auto &i) { return i.key == k; })->id;

        } else if constexpr (std::is_same<T, engine::Joystick::Axis>::value) {
            const auto map = std::to_array<SpellMap>({{engine::Joystick::LST, 2}, {engine::Joystick::RST, 3}});
            return std::find_if(map.begin(), map.end(), [&k](auto &i) { return i.key == k; })->id;

        } else { // todo : should be engine::Keyboard::Key
            const auto map = std::to_array<SpellMap>({{GLFW_KEY_U, 0}, {GLFW_KEY_Y, 1}, {GLFW_KEY_T, 2}, {GLFW_KEY_R, 3}});
            return std::find_if(map.begin(), map.end(), [&k](auto &i) { return i.key == k; })->id;
        }
    };

    auto player = m_game.player;

    std::visit(
        engine::overloaded{
            [&](const engine::Pressed<engine::Key> &key) {
                switch (key.source.key) {
                case GLFW_KEY_K: onMovement.publish(world, player, Direction::DOWN, true); break;
                case GLFW_KEY_L: onMovement.publish(world, player, Direction::RIGHT, true); break;
                case GLFW_KEY_J: onMovement.publish(world, player, Direction::LEFT, true); break;
                case GLFW_KEY_I: onMovement.publish(world, player, Direction::UP, true); break;
                case GLFW_KEY_P: m_game.setMenu(std::make_unique<menu::UpgradePanel>()); break;

                case GLFW_KEY_U:
                case GLFW_KEY_Y: {
                    const auto id = spell_map(key.source.key);

                    auto &spell = world.get<SpellSlots>(player).spells[id];
                    if (!spell.has_value()) break;

                    auto &aim = world.get<AimingDirection>(player).dir;
                    onSpellCast.publish(world, player, aim, spell.value());
                } break;
                default: return;
                }
            },
            [&](const engine::Released<engine::Key> &key) {
                switch (key.source.key) {
                case GLFW_KEY_I: onMovement.publish(world, player, Direction::UP, false); break;
                case GLFW_KEY_K: onMovement.publish(world, player, Direction::DOWN, false); break;
                case GLFW_KEY_L: onMovement.publish(world, player, Direction::RIGHT, false); break;
                case GLFW_KEY_J: onMovement.publish(world, player, Direction::LEFT, false); break;
                default: return;
                }
            },
            [&](const engine::TimeElapsed &dt) {
                onGameUpdate.publish(world, dt);
                onGameUpdateAfter.publish(world, dt);
            },
            [&](const engine::Moved<engine::JoystickAxis> &joy) {
                switch (joy.source.axis) {
                case engine::Joystick::LST:
                case engine::Joystick::RST: {
                    const auto id = spell_map(joy.source.axis);
                    auto &spell = world.get<SpellSlots>(player).spells[id];
                    if (!spell.has_value()) break;
                    auto &aim = world.get<AimingDirection>(player).dir; 
                    onSpellCast.publish(world, player, aim, spell.value());
                } break;
                case engine::Joystick::LSX:
                case engine::Joystick::LSY: {
                    auto joystick = holder.instance->getJoystick(joy.source.id);
                    auto &axis = world.get<ControllerAxis>(player).movement;

                    glm::vec2 newVal((*joystick)->axes[engine::Joystick::LSX], -(*joystick)->axes[engine::Joystick::LSY]);

                    if (glm::length(newVal) > ControllerAxis::kDeadzone)
                        axis = newVal;
                    else
                        axis = glm::vec2(0, 0);

                } break;
                case engine::Joystick::RSX:
                case engine::Joystick::RSY: {
                    auto joystick = holder.instance->getJoystick(joy.source.id);

                    auto &axis = world.get<ControllerAxis>(player).aiming;

                    glm::vec2 newVal((*joystick)->axes[engine::Joystick::RSX], -(*joystick)->axes[engine::Joystick::RSY]);

                    if (glm::length(newVal) > ControllerAxis::kDeadzone)
                        axis = newVal;
                    else
                        axis = glm::vec2(0, 0);

                } break;
                default: break;
                }
            },
            [&](const engine::Pressed<engine::JoystickButton> &joy) {
                switch (joy.source.button) {
                case engine::Joystick::CENTER2: m_game.setMenu(std::make_unique<menu::UpgradePanel>()); break;
                case engine::Joystick::LS:
                case engine::Joystick::RS: {
                    const auto id = spell_map(joy.source.button);
                    auto &spell = world.get<SpellSlots>(player).spells[id];
                    if (!spell.has_value()) break;
                    auto &aim = world.get<AimingDirection>(player).dir;
                    onSpellCast.publish(world, player, aim, spell.value());
                } break;
                default: return;
                }
            },
            [&](auto) {},
        },
        e);
}

auto game::GameLogic::slots_update_player_movement(entt::registry &world, [[maybe_unused]] const engine::TimeElapsed &dt)
    -> void
{
    auto player = m_game.player;

    auto &vel = world.get<engine::d2::Velocity>(player);
    const auto &axis = world.get<ControllerAxis>(player);
    auto spd = world.get<Speed>(player).speed;

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

        const auto result = glm::normalize(diff) * 7.f;
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
        [door = world.view<entt::tag<"exit_door"_hs>>().front(), &world, this](const auto &picker, const auto &pos) {
            if (!picker.hasKey) return;

            constexpr auto kDoorInteractionRange = 2.0;
            if (engine::d3::distance(world.get<engine::d3::Position>(door), pos) < kDoorInteractionRange)
                onFloorChange.publish(world);
        });
}

auto game::GameLogic::slots_update_camera(entt::registry &world, const engine::TimeElapsed &) -> void
{
    auto player = m_game.player;

    auto &pos = world.get<engine::d3::Position>(player);
    m_game.getCamera().setCenter({pos.x, pos.y});
    if (m_game.getCamera().isUpdated()) {
        engine::Core::Holder{}.instance->updateView(m_game.getCamera().getViewProjMatrix());
    }
}

// this function will try to update the spritesheet ate everyframe !!! BAD BAD BAD
auto game::GameLogic::slots_update_animation_spritesheet(entt::registry &world, const engine::TimeElapsed &) -> void
{
    for (const auto &i : world.view<engine::Spritesheet, engine::d2::Velocity>(entt::exclude<entt::tag<"spell"_hs>>)) {
        const auto &vel = world.get<engine::d2::Velocity>(i);
        const auto &sp = world.get<engine::Spritesheet>(i);

        const auto aiming = [](entt::registry &w, const entt::entity &e) -> std::optional<glm::vec2> {
            if (w.has<AimingDirection>(e)) {
                return w.get<AimingDirection>(e).dir;
            } else {
                return {};
            }
        }(world, i);

        const auto isFacingLeft = [](const auto &v, const auto &a) {
            if (v.x < 0)
                return true;
            else if (v.x > 0)
                return false;
            else if (a.has_value() && a.value().x < 0)
                return true;
            else
                return false;
        }(vel, aiming);

        const auto isMoving = vel.x || vel.y;

        // all the entity will be looking on the right by default because they don t have a AimingDirection BAD BAD BAD
        const auto animation = fmt::format("{}_{}", isMoving ? "run" : "idle", isFacingLeft ? "left" : "right");

        if (sp.current_animation != animation) { engine::DrawableFactory::fix_spritesheet(world, i, animation); }
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

auto game::GameLogic::slots_kill_entity(entt::registry &world, entt::entity killed, entt::entity killer) -> void
{
    static auto holder = engine::Core::Holder{};

    if (world.has<entt::tag<"player"_hs>>(killed)) {
        holder.instance->getAudioManager()
            .getSound(holder.instance->settings().data_folder + "sounds/player_death.wav")
            ->play();

        m_game.setMenu(std::make_unique<menu::GameOver>());
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
