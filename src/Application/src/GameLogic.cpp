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

game::GameLogic::GameLogic(ThePURGE &game) :
    m_game{game}, m_nextFloorSeed(static_cast<std::uint32_t>(std::time(nullptr)))
{
    sinkMovement.connect<&GameLogic::slots_move>(*this);

    sinkOnGameStarted.connect<&GameLogic::slots_game_start>(*this);

    sinkOnPlayerBuyClass.connect<&GameLogic::slots_apply_classes>(*this);
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
    sinkDamageTaken.connect<&GameLogic::slots_damage_taken>(*this);

    sinkOnFloorChange.connect<&GameLogic::slots_change_floor>(*this);
}

auto game::GameLogic::slots_game_start(entt::registry &world) -> void
{
    static auto holder = engine::Core::Holder{};

    // pos and size based of `FloorGenParam::maxDungeonWidth / Height`
    EntityFactory::create<EntityFactory::ID::BACKGROUND>(m_game, world, glm::vec2(25, 25), glm::vec2(75, 75));

    holder.instance->getAudioManager()
        .getSound(holder.instance->settings().data_folder + "sounds/entrance_gong.wav")
        ->setVolume(0.2f)
        .play();
    m_game.setBackgroundMusic("sounds/dungeon_music.wav", 0.1f);


    m_game.player = EntityFactory::create<EntityFactory::PLAYER>(m_game, world, {}, {});
    slots_apply_classes(world, m_game.player, m_game.dbClasses().getStarterClass());

    auto aimingSight = EntityFactory::create<EntityFactory::ID::AIMING_SIGHT>(m_game, world, {}, {});

    glm::vec3 playerColor(1.f, 0.2f, 0.2f);
    engine::DrawableFactory::fix_color(world, aimingSight, std::move(playerColor));
    world.get<AimSight>(m_game.player).entity = aimingSight;

    // default camera value to see the generated terrain properly
    m_game.getCamera().setCenter(glm::vec2(13, 22));
    m_game.getCamera().setViewportSize(glm::vec2(25, 17));

    onFloorChange.publish(world);
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
                case GLFW_KEY_P: {
                    m_game.setMenu(std::make_unique<menu::UpgradePanel>());
                    holder.instance->setEventMode(engine::Core::EventMode::PAUSED);
                } break;

                case GLFW_KEY_U:
                case GLFW_KEY_Y:
                case GLFW_KEY_R:
                case GLFW_KEY_T: {
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

                    const auto newVal =
                        glm::vec2{(*joystick)->axes[engine::Joystick::LSX], -(*joystick)->axes[engine::Joystick::LSY]};
                    world.get<ControllerAxis>(player).movement =
                        glm::length(newVal) > ControllerAxis::kDeadzone ? newVal : glm::vec2{0, 0};

                } break;
                case engine::Joystick::RSX:
                case engine::Joystick::RSY: {
                    auto joystick = holder.instance->getJoystick(joy.source.id);

                    const auto newVal =
                        glm::vec2{(*joystick)->axes[engine::Joystick::RSX], -(*joystick)->axes[engine::Joystick::RSY]};
                    world.get<ControllerAxis>(player).aiming =
                        glm::length(newVal) > ControllerAxis::kDeadzone ? newVal : glm::vec2{0, 0};

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
        case Particule::POSITIVE: {
            auto &color = world.get<engine::Color>(i);
            const auto r = engine::Color::r(color);
            const auto g = std::clamp(engine::Color::g(color) + 0.0001f * static_cast<float>(elapsed), 0.0f, 1.0f);
            const auto b = std::clamp(engine::Color::b(color) + 0.0001f * static_cast<float>(elapsed), 0.0f, 1.0f);
            engine::DrawableFactory::fix_color(world, i, {r, g, b});

            auto &vel = world.get<engine::d2::Velocity>(i);
            vel.x -= ((std::rand() & 1) ? -1 : 1) * 0.005 * static_cast<double>(elapsed);
            vel.y -= ((std::rand() & 1) ? -1 : 1) * 0.005 * static_cast<double>(elapsed);
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

        if (world.has<engine::Spritesheet>(i) && world.get<engine::Spritesheet>(i).current_animation == "death")
            continue;

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

auto game::GameLogic::slots_change_floor(entt::registry &world) -> void
{
    world.view<entt::tag<"terrain"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<entt::tag<"enemy"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<entt::tag<"spell"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<entt::tag<"key"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<KeyPicker>().each([&](KeyPicker &kp) { kp.hasKey = false; });

    // keep the stage instance somewhere
    const auto data = Stage{}.generate(m_game, world, m_map_generation_params, m_nextFloorSeed);
    m_nextFloorSeed = data.nextFloorSeed;

    for (const auto &player : world.view<entt::tag<"player"_hs>>()) {
        auto &pos = world.get<engine::d3::Position>(player);

        pos.x = data.spawn.x + data.spawn.w * 0.5;
        pos.y = data.spawn.y + data.spawn.h * 0.5;
    }
}
