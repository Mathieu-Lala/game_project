#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Engine/Graphics/third_party.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Event/Event.hpp>
#include <Engine/Core.hpp>

#include "level/LevelTilemapBuilder.hpp"
#include "level/MapGenerator.hpp"
#include "entity/TileFactory.hpp"

#include "EntityDepth.hpp"

#include "GameLogic.hpp"
#include "ThePURGE.hpp"

#include <stdio.h>

#ifdef __unix__
//define if linux
#elif defined(_WIN32) || defined(WIN32)

#define OS_Windows

#endif

//#include "Competences/FarmerCompetences.hpp"

using namespace std::chrono_literals;

game::ThePurge::ThePurge() : m_nextFloorSeed(static_cast<unsigned int>(std::time(nullptr))), m_logics{*this} {}

auto game::ThePurge::onDestroy(entt::registry &) -> void {}

auto game::ThePurge::onCreate([[maybe_unused]] entt::registry &world) -> void { setState(LOADING); }

auto game::ThePurge::onUpdate(entt::registry &world, const engine::Event &e) -> void
{
    static engine::Core::Holder holder{};

    if (m_state == IN_GAME) {
        std::visit(
            engine::overloaded{
                [&](const engine::Pressed<engine::Key> &key) {
                    // not really working perfectly
                    switch (key.source.key) {
                    case GLFW_KEY_UP: m_camera.move({0, 1}); break;
                    case GLFW_KEY_RIGHT: m_camera.move({1, 0}); break;
                    case GLFW_KEY_DOWN: m_camera.move({0, -1}); break;
                    case GLFW_KEY_LEFT: m_camera.move({-1, 0}); break;
                    case GLFW_KEY_O:
                        world.get<engine::d2::Acceleration>(player) = {0.0, 0.0};
                        world.get<engine::d2::Velocity>(player) = {0.0, 0.0};
                        break;                                                                     // player stop
                    case GLFW_KEY_I: m_logics.movement.publish(world, player, {0.0, 0.1}); break;  // go top
                    case GLFW_KEY_K: m_logics.movement.publish(world, player, {0.0, -0.1}); break; // go bottom
                    case GLFW_KEY_L: m_logics.movement.publish(world, player, {0.1, 0.0}); break;  // go right
                    case GLFW_KEY_J: m_logics.movement.publish(world, player, {-0.1, 0.0}); break; // go left
                    case GLFW_KEY_U: {
                        auto &vel = world.get<engine::d2::Velocity>(player);
                        m_logics.castSpell.publish(world, player, {vel.x, vel.y});
                        break;
                    }
                    default: return;
                    }
                },
                [&](const engine::TimeElapsed &dt) { m_logics.gameUpdated.publish(world, dt); },
                [&](auto) {},
            },
            e);

        if (m_camera.isUpdated()) {
            holder.instance->updateView(m_camera.getViewProjMatrix());
        }
    }
}

auto game::ThePurge::mapGenerationOverlayTick(entt::registry &world) -> void
{
    static bool spamNextFloor = false;

    ImGui::Begin("MapGeneration");

    ImGui::Checkbox("Spam next floor", &spamNextFloor);

    if (ImGui::Button("Next floor") || spamNextFloor)
        goToNextFloor(world);

    ImGui::SliderInt("Min room size", &m_map_generation_params.minRoomSize, 0, m_map_generation_params.maxRoomSize);
    ImGui::SliderInt("Max room size", &m_map_generation_params.maxRoomSize, m_map_generation_params.minRoomSize, 50);
    ImGui::Separator();

    // Assuming std::size_t is uint32_t
    ImGui::InputScalar("Min room count", ImGuiDataType_U32, &m_map_generation_params.minRoomCount);
    ImGui::InputScalar("Max room count", ImGuiDataType_U32, &m_map_generation_params.maxRoomCount);
    ImGui::Text("note: actual room count may be smaller if there is not enough space");
    ImGui::Separator();

    ImGui::DragInt("Max dungeon width", &m_map_generation_params.maxDungeonWidth, 0, 500);
    ImGui::DragInt("Max dungeon height", &m_map_generation_params.maxDungeonHeight, 0, 500);
    ImGui::Separator();

    ImGui::SliderInt(
        "Min corridor width", &m_map_generation_params.minCorridorWidth, 0, m_map_generation_params.maxCorridorWidth);
    ImGui::SliderInt(
        "Max corridor width", &m_map_generation_params.maxCorridorWidth, m_map_generation_params.minCorridorWidth, 50);
    ImGui::Separator();

    ImGui::End();
}

static bool show_demo_window = true;

auto game::ThePurge::drawUserInterface(entt::registry &world) -> void
{
    if (show_demo_window) { ImGui::ShowDemoWindow(&show_demo_window); }

    if (m_state == LOADING) {
        // todo : style because this is not a debug window
        ImGui::Begin("Menu loading", nullptr, ImGuiWindowFlags_NoDecoration);

        // note : this block could be launch in a future
        if (ImGui::Button("Start the game")) {

            player = world.create();
            world.emplace<entt::tag<"player"_hs>>(player);
            world.emplace<engine::d3::Position>(player, 0.0, 0.0, Z_COMPONENT_OF(EntityDepth::PLAYER));
            world.emplace<engine::d2::Velocity>(player, 0.0, 0.0);
            world.emplace<engine::d2::Acceleration>(player, 0.0, 0.0);
            world.emplace<engine::d2::Scale>(player, 1.0, 1.0);
            world.emplace<engine::d2::HitboxSolid>(player, 1.0, 1.0);
            world.emplace<engine::Drawable>(player, engine::DrawableFactory::rectangle());//.shader = &shader;
            engine::DrawableFactory::fix_color(world, player, {0, 0, 1});
            engine::DrawableFactory::fix_texture(world, player, DATA_DIR "textures/player.jpeg");
            world.emplace<Health>(player, 100.0f, 100.0f);
            world.emplace<AttackCooldown>(player, false, 1000ms, 0ms);
            world.emplace<AttackDamage>(player, 50.0f);
            world.emplace<Level>(player, 0u, 0u, 10u);

            // default camera value to see the generated terrain properly
            m_camera.setCenter(glm::vec2(13, 22));
            m_camera.setViewportSize(glm::vec2(109, 64));

            goToNextFloor(world);

            setState(IN_GAME);
        }

        ImGui::End();
    } else if (m_state == IN_GAME) {
        {
            ImGui::Begin("Camera");
            if (ImGui::Button("Reset")) m_camera = engine::Camera();

            auto cameraPos = m_camera.getCenter();
            ImGui::Text("Camera Position (%.3f, %.3f)", static_cast<double>(cameraPos.x), static_cast<double>(cameraPos.y));

            bool pos_updated = false;
            pos_updated |= ImGui::DragFloat("Camera X", &cameraPos.x);
            pos_updated |= ImGui::DragFloat("Camera Y", &cameraPos.y);

            if (pos_updated) m_camera.setCenter(cameraPos);

            auto viewPortSize = m_camera.getViewportSize();
            const auto pos = m_camera.getCenter();

            ImGui::Text(
                "Viewport size (%.3f, %.3f)", static_cast<double>(viewPortSize.x), static_cast<double>(viewPortSize.y));
            ImGui::Text("Viewport range :");
            ImGui::Text("   left  : %.3f", static_cast<double>(pos.x - (viewPortSize.x / 2)));
            ImGui::Text("   right : %.3f", static_cast<double>(pos.x + (viewPortSize.x / 2)));
            ImGui::Text("   top   : %.3f", static_cast<double>(pos.y + (viewPortSize.y / 2)));
            ImGui::Text("   bottom: %.3f", static_cast<double>(pos.y - (viewPortSize.y / 2)));

            bool updated = false;
            updated |= ImGui::DragFloat("Viewport width", &viewPortSize.x, 1.f, 2.f);
            updated |= ImGui::DragFloat("Viewport height", &viewPortSize.y, 1.f, 2.f);

            if (updated) m_camera.setViewportSize(viewPortSize);
            ImGui::End();
        }
        {
            const auto infoHealth = world.get<Health>(player);
            const auto HP = infoHealth.current / infoHealth.max;

            const auto level = world.get<Level>(player);
            const auto XP = static_cast<float>(level.current_xp) / static_cast<float>(level.xp_require);

            // todo : style because this is not a debug window
            ImGui::SetNextWindowPos(ImVec2(650, 20));
            ImGui::SetNextWindowSize(ImVec2(400, 100));
            ImGui::Begin(
                "Info Player",
                static_cast<bool *>(0),
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize
                    | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
            char buf[32];
#ifdef OS_Windows
            sprintf_s(buf, "%d/%d", static_cast<int>(infoHealth.current), static_cast<int>(infoHealth.max));
#else
            sprintf(buf, "%d/%d", static_cast<int>(infoHealth.current), static_cast<int>(infoHealth.max));
#endif
            ImGui::ProgressBar(HP, ImVec2(0.f, 0.f), buf);
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Text("HP");
            ImGui::ProgressBar(XP, ImVec2(0.0f, 0.0f));
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Text("XP - Level %d", level.current_level);
            ImGui::End();

            mapGenerationOverlayTick(world);
        }
    } else if (m_state == GAME_OVER) {
        // todo : style because this is not a debug window
        ImGui::Begin("Menu Game Over", nullptr, ImGuiWindowFlags_NoDecoration);

        if (ImGui::Button("Your are dead !")) {
            for (const auto &i : world.view<entt::tag<"enemy"_hs>>()) { world.destroy(i); }
            for (const auto &i : world.view<entt::tag<"terrain"_hs>>()) { world.destroy(i); }
            for (const auto &i : world.view<entt::tag<"player"_hs>>()) { world.destroy(i); }
            for (const auto &i : world.view<entt::tag<"spell"_hs>>()) { world.destroy(i); }

            setState(LOADING);
        }

        ImGui::End();
    }
}

auto game::ThePurge::goToNextFloor(entt::registry &world) -> void {
    world.view<entt::tag<"terrain"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<entt::tag<"enemy"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<entt::tag<"spell"_hs>>().each([&](auto &e) { world.destroy(e); });

    auto data = generateFloor(world, m_map_generation_params, m_nextFloorSeed);
    m_nextFloorSeed = data.nextFloorSeed;


    auto &pos = world.get<engine::d3::Position>(player);

    pos.x = data.spawn.x + data.spawn.w * 0.5;
    pos.y = data.spawn.y + data.spawn.h * 0.5;
}
