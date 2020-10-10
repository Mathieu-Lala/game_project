#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Engine/Game.hpp>
#include <Engine/Shader.hpp>
#include <Engine/Camera.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/component/Drawable.hpp>
#include <Engine/component/Position.hpp>
#include <Engine/component/Scale.hpp>
#include <Engine/component/Velocity.hpp>

#include "component/ViewRange.hpp"

#include "Declaration.hpp"
#include "level/LevelTilemapBuilder.hpp"
#include "level/TileFactory.hpp"
#include "level/MapGenerator.hpp"

class ThePurge : public engine::Game {
    engine::Shader shader =
        engine::Shader::fromFile(DATA_DIR "/shaders/camera2_vert.glsl", DATA_DIR "/shaders/camera2_frag.glsl");

    entt::entity player;
    engine::d2::Velocity *player_vel;
    engine::d2::Position *player_pos;

    auto onCreate([[maybe_unused]] entt::registry &world) -> void final
    {
        //        generateFloor(world, &shader, {}, static_cast<std::uint32_t>(::time(nullptr)));

        std::srand(static_cast<std::uint32_t>(std::time(nullptr)));
        static constexpr auto max = static_cast<double>(RAND_MAX);

//        // note : tmp generate a grid black and white (to tests coordinates systems)
//        for (auto y = -50; y != 50ul; y++)
//            for (auto x = -50; x != 50ul; x++) {
//                auto e = world.create();
//                world.emplace<engine::d2::Position>(e, x / 10.0, y / 10.0);
//                world.emplace<engine::d2::Scale>(e, 0.1, 0.1);
//                engine::Drawable drawable;
//                drawable.shader = &shader;
//                auto color = ((x & 1) && (y & 1)) || (!(x & 1) && !(y & 1));
//                engine::DrawableFactory::rectangle({color, color, color}, drawable);
//                world.emplace<engine::Drawable>(e, drawable);
//            }

        // todo : display none-terrain entity at level z=1 ?

        // note : tmp generate random entities moving on the screen (to tests velocity)
        for (int i = 0; i != 10; i++) {
            auto e = world.create();
            const auto x = 20 * ((static_cast<double>(std::rand()) / max) - 0.5);
            const auto y = 20 * ((static_cast<double>(std::rand()) / max) - 0.5);
            world.emplace<engine::d2::Position>(e, x, y);
            world.emplace<engine::d2::Velocity>(e, 0.02 * (std::rand() & 1), 0.02 * (std::rand() & 1));
            world.emplace<engine::d2::Scale>(e, 0.05, 0.05);
            world.emplace<entt::tag<"enemy"_hs>>(e);
            world.emplace<game::ViewRange>(e, 5.0f);

            engine::Drawable drawable;
            drawable.shader = &shader;
            engine::DrawableFactory::rectangle({1, 0, 0}, drawable);
            world.emplace<engine::Drawable>(e, drawable);
        }

        player = world.create();
        player_pos = &world.emplace<engine::d2::Position>(player, 0, 0);
        player_vel = &world.emplace<engine::d2::Velocity>(player, 0, 0);
        world.emplace<engine::d2::Scale>(player, 0.07, 0.07);
        engine::Drawable drawable;
        drawable.shader = &shader;
        engine::DrawableFactory::rectangle({1, 1, 1}, drawable);
        world.emplace<engine::Drawable>(player, drawable);
    }

    auto onUpdate([[maybe_unused]] entt::registry &world, const engine::Event &e) -> void final
    {
        static engine::Core::Holder holder;

        std::visit(
            engine::overloaded{
//                [&](const engine::Pressed<engine::MouseButton> &m) {
//                    // note : this is not working
//                    player_pos->x = m.source.mouse.x / holder.instance->window()->getSize().x - 0.5;
//                    player_pos->y = m.source.mouse.y / holder.instance->window()->getSize().y - 0.5;
//                },
                [&](const engine::Pressed<engine::Key> &key) {
                    // spdlog::info("key pressed {}", key.source.key);

                    // not really working perfectly
                    switch (key.source.key) {
                    case GLFW_KEY_W: m_camera.move(engine::Camera_Movement::FORWARD, 0.1f); break;
                    case GLFW_KEY_A: m_camera.move(engine::Camera_Movement::LEFT, 0.1f); break;
                    case GLFW_KEY_S: m_camera.move(engine::Camera_Movement::BACKWARD, 0.1f); break;
                    case GLFW_KEY_D: m_camera.move(engine::Camera_Movement::RIGHT, 0.1f); break;
                    case GLFW_KEY_UP: m_camera.turn(0, 1); break;
                    case GLFW_KEY_RIGHT: m_camera.turn(1, 0); break;
                    case GLFW_KEY_DOWN: m_camera.turn(0, -1); break;
                    case GLFW_KEY_LEFT: m_camera.turn(-1, 0); break;
                    case GLFW_KEY_I: *player_vel = {0, 0.7}; break;
                    case GLFW_KEY_J: *player_vel = {-0.7, 0}; break;
                    case GLFW_KEY_K: *player_vel = {0, -0.7}; break;
                    case GLFW_KEY_L: *player_vel = {0.7, 0}; break;
                    default: return;
                    }
                },
                [&](const engine::TimeElapsed &) {
                    world.view<entt::tag<"enemy"_hs>, engine::d2::Position, engine::d2::Velocity, game::ViewRange>()
                    .each([&](auto &, auto &pos, auto &vel, auto &view_range) {

                        const glm::vec2 diff = { player_pos->x - pos.x, player_pos->y - pos.y };

                        // if the enemy is close enough
                        if (glm::length(diff) <= view_range.range) {
                            vel = { diff.x, diff.y };
                        } else {
                            vel = { 0, 0 };
                        }

                    });
                },
                [&](auto) {},
            },
            e);

        // todo : only send them to the shader if camera updated

        const auto projection = glm::perspective(glm::radians(1.0), holder.instance->window()->getAspectRatio(), 0.1, 1000.0);
        shader.uploadUniformMat4("projection", projection);

        const auto view = m_camera.GetViewMatrix();
        shader.uploadUniformMat4("view", view);
    }

    auto onDestroy(entt::registry &) -> void final {}

    float camera_zoom_level = 0;

    bool show_demo_window = true;
    auto drawUserInterface() -> void final
    {
        if (show_demo_window) { ImGui::ShowDemoWindow(&show_demo_window); }

        ImGui::Begin("Camera");

        ImGui::Text("Camera Position (%.3f, %.3f, %.3f)", m_camera.Position.x, m_camera.Position.y, m_camera.Position.z);
        ImGui::Text("Camera Rotation (%.3f, %.3f, %.3f)", m_camera.Yaw, m_camera.Pitch, 0.0);

        // note : this is not working smoothly as expected
        if (ImGui::SliderFloat("Camera Z", &camera_zoom_level, 0.0f, 100.0f, "z = %.3f")) {
            m_camera = engine::Camera{glm::vec3(0.0f, 0.0f, camera_zoom_level)};
        }

        ImGui::End();
    }

private:
    engine::Camera m_camera{glm::vec3(0.0f, 0.0f, 100.0f)};
};
