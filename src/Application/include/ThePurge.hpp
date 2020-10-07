#pragma once

#include <Engine/Game.hpp>
#include <Engine/Shader.hpp>
#include <Engine/component/Drawable.hpp>

#include "Declaration.hpp"
#include "level/LevelTilemapBuilder.hpp"
#include "level/TileFactory.hpp"
#include "level/MapGenerator.hpp"

class ThePurge : public engine::Game {

    glm::vec4 screen_pos = { 0, 89, 0, 50 };

    auto onCreate(entt::registry &world) -> void final
    {
        generateFloor(world, {}, static_cast<std::uint32_t>(::time(nullptr)));

        m_camera.setViewport(screen_pos.x, screen_pos.y, screen_pos.z, screen_pos.w);
        m_camera.setCenter({0, 0});

        TileFactory::getShader()->uploadUniformMat4("u_ViewProjection", m_camera.getViewProjMatrix());
    }

    auto onDestroy(entt::registry &) -> void final
    {
    }

    auto onUpdate(entt::registry &world, const engine::Event &e) -> void final
    {
        std::visit(engine::overloaded{
            [&](const engine::Pressed<engine::Key> &key) {
                spdlog::info("key pressed {}", key.source.key);

                // note : this does not work as expected ..
                switch (key.source.key) {
                case GLFW_KEY_UP: screen_pos += glm::vec4(-10, 10, 0, 0); break;
                case GLFW_KEY_RIGHT: screen_pos += glm::vec4(10, -10, 0, 0); break;
                case GLFW_KEY_DOWN: screen_pos += glm::vec4(0, 0, 10, -10); break;
                case GLFW_KEY_LEFT: screen_pos += glm::vec4(0, 0, -10, 10); break;
                default: break;
                }
                m_camera.setViewport(screen_pos.x, screen_pos.y, screen_pos.z, screen_pos.w);
                TileFactory::getShader()->uploadUniformMat4("u_ViewProjection", m_camera.getViewProjMatrix());

            },
            [&](const engine::TimeElapsed &t) { onTick(world, t.elapsed); },
            [](auto) { },
        }, e);
    }

    bool show_demo_window = true;

    auto drawUserInterface() -> void final
    {
        if (show_demo_window) { ImGui::ShowDemoWindow(&show_demo_window); }
    }

private:

    engine::Camera m_camera;

    auto onTick(entt::registry &, const std::chrono::steady_clock::duration &) -> void
    {
    }

};
