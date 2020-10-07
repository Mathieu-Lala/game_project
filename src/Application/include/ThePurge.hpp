#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Engine/Game.hpp>
#include <Engine/Shader.hpp>
#include <Engine/component/Drawable.hpp>
#include <Engine/component/Position.hpp>
#include <Engine/helpers/DrawableFactory.hpp>

#include <Engine/Camera2.hpp>

#include "Declaration.hpp"
#include "level/LevelTilemapBuilder.hpp"
#include "level/TileFactory.hpp"
#include "level/MapGenerator.hpp"

class ThePurge : public engine::Game {

    glm::vec4 screen_pos = { 0, 89, 0, 50 };

    engine::Shader shader = engine::Shader::fromFile(DATA_DIR "/shaders/camera_vert.glsl", DATA_DIR "/shaders/camera_frag.glsl");

    auto onCreate(entt::registry &world) -> void final
    {
//        generateFloor(world, {}, static_cast<std::uint32_t>(::time(nullptr)));
//
//        m_camera.setViewport(screen_pos.x, screen_pos.y, screen_pos.z, screen_pos.w);
//        m_camera.setCenter({0, 0});
//
//        TileFactory::getShader()->uploadUniformMat4("u_ViewProjection", m_camera.getViewProjMatrix());

        auto e = world.create();
        world.emplace<engine::_2d::Position>(e, 0, 0);
        world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle(
            {0, 0}, {0.1, 0.1}, glm::vec3(1, 0, 1), &shader));
    }

    auto onDestroy(entt::registry &) -> void final
    {
    }

    auto onUpdate(entt::registry &world, const engine::Event &e) -> void final
    {
        glm::mat4 projection = glm::perspective(glm::radians(m_camera.Zoom),
            engine::Core::Holder{}.instance->window()->getAspectRatio(), 0.1f, 100.0f);
        shader.uploadUniformMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = m_camera.GetViewMatrix();
        shader.uploadUniformMat4("view", view);

        std::visit(engine::overloaded{
            [&](const engine::Pressed<engine::Key> &key) {
                spdlog::info("key pressed {}", key.source.key);

//                // note : this does not work as expected ..
//                switch (key.source.key) {
//                case GLFW_KEY_UP: screen_pos += glm::vec4(-10, 10, 0, 0); break;
//                case GLFW_KEY_RIGHT: screen_pos += glm::vec4(10, -10, 0, 0); break;
//                case GLFW_KEY_DOWN: screen_pos += glm::vec4(0, 0, 10, -10); break;
//                case GLFW_KEY_LEFT: screen_pos += glm::vec4(0, 0, -10, 10); break;
//                default: break;
//                }
//                m_camera.setViewport(screen_pos.x, screen_pos.y, screen_pos.z, screen_pos.w);
//                TileFactory::getShader()->uploadUniformMat4("u_ViewProjection", m_camera.getViewProjMatrix());

                world.view<engine::_2d::Position>().each([&](auto &pos){
                    switch (key.source.key) {
                        case GLFW_KEY_UP: pos.y += 0.1f; break;
                        case GLFW_KEY_RIGHT: pos.x += 0.1f; break;
                        case GLFW_KEY_DOWN: pos.y += -0.1f; break;
                        case GLFW_KEY_LEFT: pos.x += -0.1f; break;
                        default: return;
                    }
                });

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

//    engine::Camera m_camera;
    engine::exp::Camera m_camera{glm::vec3(0.0f, 0.0f, 3.0f)};

    auto onTick(entt::registry &world, const std::chrono::steady_clock::duration &) -> void
    {
        world.view<engine::Drawable, engine::_2d::Position>().each([&](auto &drawable, auto &pos){

            drawable.shader->use();
            const auto model = glm::translate(glm::mat4(1.0f), glm::vec3{pos.x, pos.y, 0.0f});
//              float angle = 20.0f * i;
//              model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            drawable.shader->uploadUniformMat4("model", model);

            ::glBindVertexArray(drawable.VAO);
            ::glDrawElements(GL_TRIANGLES, 3 * drawable.triangle_count, GL_UNSIGNED_INT, 0);
        });
    }

};
