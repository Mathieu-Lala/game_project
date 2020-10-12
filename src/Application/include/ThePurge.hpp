#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Engine/Game.hpp>
#include <Engine/Shader.hpp>
#include <Engine/Camera2d.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/component/Drawable.hpp>
#include <Engine/component/Position.hpp>
#include <Engine/component/Scale.hpp>
#include <Engine/component/Velocity.hpp>
#include <Engine/component/Acceleration.hpp>
#include <Engine/component/Hitbox.hpp>

#include "component/ViewRange.hpp"
#include "component/AttackRange.hpp"
#include "component/AttackDamage.hpp"
#include "component/AttackCooldown.hpp"
#include "component/Health.hpp"

#include "Declaration.hpp"
#include "level/LevelTilemapBuilder.hpp"
#include "level/TileFactory.hpp"
#include "level/MapGenerator.hpp"

class ThePurge : public engine::Game {
    engine::Shader shader =
        engine::Shader::fromFile(DATA_DIR "/shaders/camera.vert.glsl", DATA_DIR "/shaders/camera.frag.glsl");

    entt::entity player;

    auto onCreate([[maybe_unused]] entt::registry &world) -> void final
    {
        using namespace std::chrono_literals; // ms ..

        // generateFloor(world, &shader, {}, static_cast<std::uint32_t>(::time(nullptr)));

        std::srand(static_cast<std::uint32_t>(std::time(nullptr)));
        static constexpr auto max = static_cast<double>(RAND_MAX);

        // todo : display none-terrain entity at level z=1 ?

        // note : tmp generate random entities moving on the screen (to tests velocity)
        for (int i = 0; i != 20; i++) {
            auto e = world.create();
            const auto x = 50 * ((static_cast<double>(std::rand()) / max) - 0.5);
            const auto y = 50 * ((static_cast<double>(std::rand()) / max) - 0.5);
            world.emplace<entt::tag<"enemy"_hs>>(e);
            world.emplace<engine::d2::Position>(e, x, y);
            world.emplace<engine::d2::Velocity>(e, 0.02 * (std::rand() & 1), 0.02 * (std::rand() & 1));
            world.emplace<engine::d2::Scale>(e, 1.0, 1.0);
            world.emplace<engine::d2::Hitbox>(e, 1.0, 1.0);
            world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle({1, 0, 0})).shader = &shader;
            world.emplace<game::ViewRange>(e, 10.0f);
            world.emplace<game::AttackRange>(e, 3.0f);
            world.emplace<game::AttackCooldown>(e, false, 4000ms, 0ms);
            world.emplace<game::AttackDamage>(e, 20.0f);
        }

        player = world.create();
        world.emplace<engine::d2::Position>(player, 0.0, 0.0);
        world.emplace<engine::d2::Velocity>(player, 0.0, 0.0);
        world.emplace<engine::d2::Acceleration>(player, 0.0, 0.0);
        world.emplace<engine::d2::Scale>(player, 1.0, 1.0);
        world.emplace<engine::d2::Hitbox>(player, 1.0, 1.0);
        world.emplace<engine::Drawable>(player, engine::DrawableFactory::rectangle({1, 1, 1})).shader = &shader;
        world.emplace<game::Health>(player, 100.0f, 100.0f);

        // generateFloor(world, &shader, {}, static_cast<unsigned int>(time(nullptr)));
    }

    auto onUpdate([[maybe_unused]] entt::registry &world, const engine::Event &e) -> void final
    {
        static engine::Core::Holder holder;

        std::visit(
            engine::overloaded{
                [&](const engine::Pressed<engine::Key> &key) {
                    // spdlog::info("key pressed {}", key.source.key);

                    // not really working perfectly
                    switch (key.source.key) {
                    case GLFW_KEY_UP: m_camera.move({0, 1}); break;
                    case GLFW_KEY_RIGHT: m_camera.move({1, 0}); break;
                    case GLFW_KEY_DOWN: m_camera.move({0, -1}); break;
                    case GLFW_KEY_LEFT: m_camera.move({-1, 0}); break;
                    case GLFW_KEY_O:
                        world.get<engine::d2::Acceleration>(player) = {0.0, 0.0};
                        world.get<engine::d2::Velocity>(player) = {0.0, 0.0};
                        break;                                                                         // player stop
                    case GLFW_KEY_I: world.get<engine::d2::Acceleration>(player) = {0.0, 0.1}; break;  // go top
                    case GLFW_KEY_J: world.get<engine::d2::Acceleration>(player) = {-0.1, 0.0}; break; // go left
                    case GLFW_KEY_K: world.get<engine::d2::Acceleration>(player) = {0.0, -0.1}; break; // go bottom
                    case GLFW_KEY_L: world.get<engine::d2::Acceleration>(player) = {0.1, 0.0}; break;  // go right
                    default: return;
                    }
                },
                [&](const engine::TimeElapsed &dt) {
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

                    world.view<game::AttackCooldown>().each([&](auto &attack_cooldown) {
                        if (!attack_cooldown.is_in_cooldown) return;

                        if (dt.elapsed < attack_cooldown.remaining_cooldown) {
                            attack_cooldown.remaining_cooldown -=
                                std::chrono::duration_cast<std::chrono::milliseconds>(dt.elapsed);
                        } else {
                            attack_cooldown.is_in_cooldown = false;
                            spdlog::warn("attack is up !");
                        }
                    });

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
                },
                [&](auto) {},
            },
            e);


        const auto viewProj = m_camera.getViewProjMatrix();
        shader.uploadUniformMat4("viewProj", viewProj);
    }

    auto mapGenerationOverlayTick(entt::registry &world) -> void
    {
        static FloorGenParam params;

        ImGui::Begin("MapGeneration");

        if (ImGui::Button("Generate")) generateFloor(world, &shader, params); // TODO: check how to handle seeding
        if (ImGui::Button("Despawn")) {
            world.view<entt::tag<"terrain"_hs>>().each([&](auto &e) { world.destroy(e); });
        }

        ImGui::SliderInt("Min room size", &params.minRoomSize, 0, params.maxRoomSize);
        ImGui::SliderInt("Max room size", &params.maxRoomSize, params.minRoomSize, 50);
        ImGui::Separator();

        // Assuming std::size_t is uint32_t
        ImGui::InputScalar("Min room count", ImGuiDataType_U32, &params.minRoomCount);
        ImGui::InputScalar("Max room count", ImGuiDataType_U32, &params.maxRoomCount);
        ImGui::Text("note: actual room count may be smaller if there is not enough space");
        ImGui::Separator();

        ImGui::DragInt("Max dungeon width", &params.maxDungeonWidth, 0, 500);
        ImGui::DragInt("Max dungeon height", &params.maxDungeonHeight, 0, 500);
        ImGui::Separator();

        ImGui::SliderInt("Min corridor width", &params.minCorridorWidth, 0, params.maxCorridorWidth);
        ImGui::SliderInt("Max corridor width", &params.maxCorridorWidth, params.minCorridorWidth, 50);
        ImGui::Separator();

        ImGui::End();
    }

    auto onDestroy(entt::registry &) -> void final {}

    bool show_demo_window = true;
    auto drawUserInterface(entt::registry &world) -> void final
    {
        if (show_demo_window) { ImGui::ShowDemoWindow(&show_demo_window); }

        ImGui::Begin("Camera");

        if (ImGui::Button("Reset")) m_camera = engine::Camera2d();

        {
            auto cameraPos = m_camera.getCenter();
            ImGui::Text("Camera Position (%.3f, %.3f)", cameraPos.x, cameraPos.y);

            bool dirty = false;
            dirty |= ImGui::SliderFloat("Camera X", &cameraPos.x, -5.0f, 5.0f, "x = %.3f");
            dirty |= ImGui::SliderFloat("Camera Y", &cameraPos.y, -5.0f, 5.0f, "y = %.3f");

            if (dirty) m_camera.setCenter(cameraPos);
        }


        auto cameraZ = m_camera.getZ();
        if (ImGui::SliderFloat("Camera Z", &cameraZ, -5.0f, 5.0f, "z = %.3f")) m_camera.setZ(cameraZ);

        {
            auto viewPortSize = m_camera.getViewportSize();
            auto pos = m_camera.getCenter();

            ImGui::Text("Viewport size (%.3f, %.3f)", viewPortSize.x, viewPortSize.y);
            ImGui::Text("Viewport range :");
            ImGui::Text("   left  : %.3f", pos.x - (viewPortSize.x / 2));
            ImGui::Text("   right : %.3f", pos.x + (viewPortSize.x / 2));
            ImGui::Text("   top   : %.3f", pos.y + (viewPortSize.y / 2));
            ImGui::Text("   bottom: %.3f", pos.y - (viewPortSize.y / 2));


            bool dirty = false;
            dirty |= ImGui::SliderFloat("Viewport width", &viewPortSize.x, 0.1f, 50.0f);
            dirty |= ImGui::SliderFloat("Viewport height", &viewPortSize.y, 0.1f, 50.0f);

            if (dirty) m_camera.setViewportSize(viewPortSize);
        }


        ImGui::End();

        mapGenerationOverlayTick(world);
    }

private:
    engine::Camera2d m_camera;
};
