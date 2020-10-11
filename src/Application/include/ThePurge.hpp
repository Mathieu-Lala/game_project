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
#include <Engine/component/Acceleration.hpp>
#include <Engine/component/Hitbox.hpp>

#include "component/ViewRange.hpp"

#include "Declaration.hpp"
#include "level/LevelTilemapBuilder.hpp"
#include "level/TileFactory.hpp"
#include "level/MapGenerator.hpp"

class ThePurge : public engine::Game {
    engine::Shader shader =
        engine::Shader::fromFile(DATA_DIR "/shaders/camera.vert.glsl", DATA_DIR "/shaders/camera.frag.glsl");

    entt::entity player;
    engine::d2::Velocity *player_vel;
    engine::d2::Position *player_pos;

    auto onCreate([[maybe_unused]] entt::registry &world) -> void final
    {
        // generateFloor(world, &shader, {}, static_cast<std::uint32_t>(::time(nullptr)));

        std::srand(static_cast<std::uint32_t>(std::time(nullptr)));
        static constexpr auto max = static_cast<double>(RAND_MAX);

        // todo : display none-terrain entity at level z=1 ?

        // note : tmp generate random entities moving on the screen (to tests velocity)
        for (int i = 0; i != 3; i++) {
            auto e = world.create();
            const auto x = 20 * ((static_cast<double>(std::rand()) / max) - 0.5);
            const auto y = 20 * ((static_cast<double>(std::rand()) / max) - 0.5);
            world.emplace<engine::d2::Position>(e, x, y);
            world.emplace<engine::d2::Velocity>(e, 0.02 * (std::rand() & 1), 0.02 * (std::rand() & 1));
            world.emplace<engine::d2::Scale>(e, 0.05, 0.05);
            world.emplace<entt::tag<"enemy"_hs>>(e);
            world.emplace<game::ViewRange>(e, 8.0f);
            world.emplace<engine::d2::Hitbox>(e, 2.0, 2.0);
            world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle({1, 0, 0})).shader = &shader;
        }

        player = world.create();
        player_pos = &world.emplace<engine::d2::Position>(player, 0.0, 0.0);
        player_vel = &world.emplace<engine::d2::Velocity>(player, 0.0, 0.0);
        world.emplace<engine::d2::Acceleration>(player, 0.0, 0.0);
        world.emplace<engine::d2::Scale>(player, 0.05, 0.05);
        world.emplace<engine::d2::Hitbox>(player, 2.0, 2.0);
        world.emplace<engine::Drawable>(player, engine::DrawableFactory::rectangle({1, 1, 1})).shader = &shader;
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
                    case GLFW_KEY_UP: m_camera.turn(0, 1); break;
                    case GLFW_KEY_RIGHT: m_camera.turn(1, 0); break;
                    case GLFW_KEY_DOWN: m_camera.turn(0, -1); break;
                    case GLFW_KEY_LEFT: m_camera.turn(-1, 0); break;
                    case GLFW_KEY_O:
                        world.get<engine::d2::Acceleration>(player) = {0.0, 0.0};
                        world.get<engine::d2::Velocity>(player) = {0.0, 0.0};
                        break; // player stop
                    case GLFW_KEY_I: world.get<engine::d2::Acceleration>(player) = {0.0, 0.1}; break; // go top
                    case GLFW_KEY_J: world.get<engine::d2::Acceleration>(player) = {-0.1, 0.0}; break; // go left
                    case GLFW_KEY_K: world.get<engine::d2::Acceleration>(player) = {0.0, -0.1}; break; // go bottom
                    case GLFW_KEY_L: world.get<engine::d2::Acceleration>(player) = {0.1, 0.0}; break; // go right
                    default: return;
                    }
                },
                [&](const engine::TimeElapsed &) {
//                    spdlog::info("player {} {}", player_pos->x, player_pos->y);
                    world.view<entt::tag<"enemy"_hs>, engine::d2::Position, engine::d2::Velocity, game::ViewRange>()
                    .each([&](auto &, auto &pos, auto &vel, auto &view_range) {

                        const glm::vec2 diff = { player_pos->x - pos.x, player_pos->y - pos.y };

                        // if the enemy is close enough
                        if (glm::length(diff) <= view_range.range) {
                            vel = { diff.x, diff.y };
//                            spdlog::info("enemy {} {}", pos.x, pos.y);
                        } else {
                            vel = { 0, 0 };
                        }

                    });
                },
                [&](auto) {},
            },
            e);


        // todo : only send them to the shader if camera updated
        const auto projection = glm::perspective(glm::radians(m_camera.Zoom),
            holder.instance->window()->getAspectRatio(), 0.1, 1000.0);

//        const auto projection = glm::ortho(0.0, holder.instance->window()->getSize().x,
//            0.0, holder.instance->window()->getSize().y, 0.1, 1000.0);

        shader.uploadUniformMat4("projection", projection);

        const auto view = m_camera.GetViewMatrix();
        shader.uploadUniformMat4("view", view);
    }

    auto onDestroy(entt::registry &) -> void final {}

    bool show_demo_window = true;
    auto drawUserInterface() -> void final
    {
        if (show_demo_window) { ImGui::ShowDemoWindow(&show_demo_window); }

        ImGui::Begin("Camera");

        ImGui::Text("Camera Position (%.3f, %.3f, %.3f)", m_camera.Position.x, m_camera.Position.y, m_camera.Position.z);

        // ugly af

        static float camera_x = static_cast<float>(m_camera.Position.x);
        static float camera_y = static_cast<float>(m_camera.Position.y);
        static float camera_z = static_cast<float>(m_camera.Position.z);

        // todo : move the camera instead of creating a new object
        if (ImGui::SliderFloat("Camera X", &camera_x, -5.0f, 5.0f, "x = %.3f")) {
            m_camera = engine::Camera{glm::vec3(camera_x, camera_y, camera_z)};
        }
        if (ImGui::SliderFloat("Camera Y", &camera_y, -5.0f, 5.0f, "y = %.3f")) {
            m_camera = engine::Camera{glm::vec3(camera_x, camera_y, camera_z)};
        }
        if (ImGui::SliderFloat("Camera Z", &camera_z, -5.0f, 5.0f, "z = %.3f")) {
            m_camera = engine::Camera{glm::vec3(camera_x, camera_y, camera_z)};
        }

        ImGui::Text("Camera Rotation (%.3f, %.3f, %.3f)", m_camera.Yaw, m_camera.Pitch, 0.0);

        static float fov = 1.0f;

        ImGui::Text("Camera Zoom (%.3f)", m_camera.Zoom);
        if (ImGui::SliderFloat("Camera Zoom", &fov, 1.0f, 45.0f, "fov = %.3f")) {
            m_camera.setZoom(fov);
        }

        ImGui::End();
    }

private:
    engine::Camera m_camera{glm::vec3(0.0f, 0.0f, 3.0f)};
};
