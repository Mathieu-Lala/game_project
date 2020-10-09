#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <Engine/Game.hpp>
#include <Engine/Shader.hpp>
#include <Engine/component/Drawable.hpp>
#include <Engine/component/Position.hpp>
#include <Engine/helpers/DrawableFactory.hpp>

#include <Engine/Camera.hpp>

#include "Declaration.hpp"
#include "level/LevelTilemapBuilder.hpp"
#include "level/TileFactory.hpp"
#include "level/MapGenerator.hpp"

class ThePurge : public engine::Game {
    engine::Shader shader =
        engine::Shader::fromFile(DATA_DIR "/shaders/camera2_vert.glsl", DATA_DIR "/shaders/camera2_frag.glsl");

    auto onCreate([[maybe_unused]] entt::registry &world) -> void final
    {
        generateFloor(world, &shader, {}, static_cast<std::uint32_t>(::time(nullptr)));

        std::srand(static_cast<std::uint32_t>(std::time(nullptr)));

        static constexpr auto max = static_cast<double>(RAND_MAX);

        for (int i = 0; i != 10; i++) {
            auto e = world.create();
            const auto x = 10 * ((static_cast<double>(std::rand()) / max) - 0.5);
            const auto y = 10 * ((static_cast<double>(std::rand()) / max) - 0.5);
            auto color = glm::vec3(std::rand() & 1, std::rand() & 1, std::rand() & 1);

            world.emplace<engine::d2::Position>(e, x, y);
            world.emplace<engine::d2::Velocity>(e, 0.01 * (std::rand() & 1), 0.01 * (std::rand() & 1));
            world.emplace<engine::d2::Scale>(e, 0.05, 0.05);
            engine::Drawable drawable;
            drawable.shader = &shader;
            engine::DrawableFactory::rectangle(std::move(color), drawable);
            world.emplace<engine::Drawable>(e, drawable);
        }
    }

    auto onUpdate([[maybe_unused]] entt::registry &world, const engine::Event &e) -> void final
    {
        std::visit(
            engine::overloaded{
                [&](const engine::Pressed<engine::Key> &key) {
                    spdlog::info("key pressed {}", key.source.key);

                    // not really working perfectly
                    switch (key.source.key) {
                    case GLFW_KEY_W: m_camera.move(engine::Camera_Movement::FORWARD, 0.1f); break;
                    case GLFW_KEY_A:
                        m_camera.move(engine::Camera_Movement::LEFT, 0.1f);
                        ;
                        break;
                    case GLFW_KEY_S: m_camera.move(engine::Camera_Movement::BACKWARD, 0.1f); break;
                    case GLFW_KEY_D: m_camera.move(engine::Camera_Movement::RIGHT, 0.1f); break;
                    case GLFW_KEY_UP: m_camera.turn(0, 1); break;
                    case GLFW_KEY_RIGHT: m_camera.turn(1, 0); break;
                    case GLFW_KEY_DOWN: m_camera.turn(0, -1); break;
                    case GLFW_KEY_LEFT: m_camera.turn(-1, 0); break;
                    default: return;
                    }
                },
                [&](auto) {},
            },
            e);

        static engine::Core::Holder holder;

        auto projection = glm::perspective(glm::radians(1.0), holder.instance->window()->getAspectRatio(), 0.1, 1000.0);
        shader.uploadUniformMat4("projection", projection);

        auto view = m_camera.GetViewMatrix();
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
