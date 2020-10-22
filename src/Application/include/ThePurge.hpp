#pragma once

#include <entt/entt.hpp>

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
#include <Engine/GameLogic.hpp>
#include <Engine/DataConfigLoader.hpp>

#include "component/ViewRange.hpp"
#include "component/AttackRange.hpp"
#include "component/AttackDamage.hpp"
#include "component/AttackCooldown.hpp"
#include "component/Health.hpp"

#include "Declaration.hpp"
#include "level/LevelTilemapBuilder.hpp"
#include "level/MapGenerator.hpp"
#include "entity/TileFactory.hpp"

 #include "EntityDepth.hpp"

class ThePurge : public engine::Game {
    engine::Shader shader =
        engine::Shader::fromFile(DATA_DIR "/shaders/camera.vert.glsl", DATA_DIR "/shaders/camera.frag.glsl");

    entt::entity player;

    engine::DataConfigLoader loader;

    // Init movement signal player
    entt::sigh<void(entt::registry &, entt::entity &, const engine::d2::Acceleration &)> movement;
    entt::sink<void(entt::registry &, entt::entity &, const engine::d2::Acceleration &)> sinkMovement{movement};

    // entityLogic signal loop
    entt::sigh<void(entt::registry &, entt::entity &, const engine::TimeElapsed &)> gameLogic;
    entt::sink<void(entt::registry &, entt::entity &, const engine::TimeElapsed &)> sinkGameLogic{gameLogic};

    entt::sigh<void(void)> castSpell;
    entt::sink<void(void)> sinkCastSpell{castSpell};

    auto onCreate([[maybe_unused]] entt::registry &world) -> void final
    {
        using namespace std::chrono_literals; // ms ..

        // generateFloor(world, &shader, {}, static_cast<std::uint32_t>(::time(nullptr)));

        std::srand(static_cast<std::uint32_t>(std::time(nullptr)));
        // static constexpr auto max = static_cast<double>(RAND_MAX);

        // todo : display none-terrain entity at level z=1 ?

        // Connect the movement signal to the corresponding system
        sinkMovement.connect<&engine::GameLogic::move>(engine::GameLogic());

        sinkGameLogic.connect<&engine::GameLogic::collision>(engine::GameLogic());
        sinkGameLogic.connect<&engine::GameLogic::cooldown>(engine::GameLogic());
        sinkGameLogic.connect<&engine::GameLogic::attack>(engine::GameLogic());

        sinkCastSpell.connect<&engine::GameLogic::castSpell>(engine::GameLogic());

        // Test json loader

        player = world.create();

        loader.loadPlayerConfigFile(std::string_view(DATA_DIR "json/player.json"), world, player, shader);
        // loader.loadClassConfigFile(std::string_view(DATA_DIR "json/classes.json"), world, player, engine::Classes::FARMER);
        loader.loadClassConfigFile(std::string_view(DATA_DIR "json/classes.json"), world, player, engine::Classes::SHOOTER);
        // default camera value to see the generated terrain properly
        m_camera.setCenter(glm::vec2(13, 22));
        m_camera.setViewportSize(glm::vec2(109, 64));
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
                        break; // player stop
                    case GLFW_KEY_I: movement.publish(world, player, {0.0, 0.1}); break; // go top
                    case GLFW_KEY_K: movement.publish(world, player, {0.0, -0.1}); break; // go bottom
                    case GLFW_KEY_L: movement.publish(world, player, {0.1, 0.0}); break; // go right
                    case GLFW_KEY_J: movement.publish(world, player, {-0.1, 0.0}); break; // go left
                    case GLFW_KEY_S: castSpell.publish(); break;
                    default: return;
                    }
                },
                [&](const engine::TimeElapsed &dt) {
                    gameLogic.publish(world, player, dt);
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

        if (ImGui::Button("Generate")) {
            auto data = generateFloor(world, &shader, params, static_cast<unsigned int>(std::time(nullptr))); // TODO: check how to handle seeding better
            auto &pos = world.get<engine::d2::Position>(player);

            pos.x = data.spawn.x + data.spawn.w * 0.5;
            pos.y = data.spawn.y + data.spawn.h * 0.5;
        }
        if (ImGui::Button("Despawn")) {
            world.view<entt::tag<"terrain"_hs>>().each([&](auto &e) { world.destroy(e); });
            world.view<entt::tag<"enemy"_hs>>().each([&](auto &e) { world.destroy(e); });
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
            ImGui::Text("Camera Position (%.3f, %.3f)", static_cast<double>(cameraPos.x), static_cast<double>(cameraPos.y));

            bool dirty = false;
            dirty |= ImGui::DragFloat("Camera X", &cameraPos.x);
            dirty |= ImGui::DragFloat("Camera Y", &cameraPos.y);

            if (dirty) m_camera.setCenter(cameraPos);
        }

        {
            auto viewPortSize = m_camera.getViewportSize();
            auto pos = m_camera.getCenter();

            ImGui::Text("Viewport size (%.3f, %.3f)", static_cast<double>(viewPortSize.x), static_cast<double>(viewPortSize.y));
            ImGui::Text("Viewport range :");
            ImGui::Text("   left  : %.3f", static_cast<double>(pos.x - (viewPortSize.x / 2)));
            ImGui::Text("   right : %.3f", static_cast<double>(pos.x + (viewPortSize.x / 2)));
            ImGui::Text("   top   : %.3f", static_cast<double>(pos.y + (viewPortSize.y / 2)));
            ImGui::Text("   bottom: %.3f", static_cast<double>(pos.y - (viewPortSize.y / 2)));


            bool dirty = false;
            dirty |= ImGui::DragFloat("Viewport width", &viewPortSize.x, 1.f, 2.f);
            dirty |= ImGui::DragFloat("Viewport height", &viewPortSize.y, 1.f, 2.f);


            if (dirty) m_camera.setViewportSize(viewPortSize);
        }


        ImGui::End();

        mapGenerationOverlayTick(world);
    }

private:
    engine::Camera2d m_camera;
};
