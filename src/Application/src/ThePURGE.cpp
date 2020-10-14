#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Graphics/third_party.hpp>

#include "GameLogic.hpp"
#include "level/LevelTilemapBuilder.hpp"
#include "level/MapGenerator.hpp"
#include "entity/TileFactory.hpp"

#include "EntityDepth.hpp"

#include "ThePURGE.hpp"

//#include "Competences/FarmerCompetences.hpp"

auto game::ThePurge::onDestroy(entt::registry &) -> void {}

auto game::ThePurge::onCreate(entt::registry &world) -> void
{
    sinkMovement.connect<&GameLogic::move>(GameLogic());

    sinkGameLogic.connect<&GameLogic::collision>(GameLogic());
    sinkGameLogic.connect<&GameLogic::cooldown>(GameLogic());
    sinkGameLogic.connect<&GameLogic::attack>(GameLogic());

    sinkCastSpell.connect<&GameLogic::castSpell>(GameLogic());


    player = world.create();
    world.emplace<engine::d3::Position>(player, 0.0, 0.0, Z_COMPONENT_OF(EntityDepth::PLAYER));
    world.emplace<engine::d2::Velocity>(player, 0.0, 0.0);
    world.emplace<engine::d2::Acceleration>(player, 0.0, 0.0);
    world.emplace<engine::d2::Scale>(player, 1.0, 1.0);
    world.emplace<engine::d2::Hitbox>(player, 1.0, 1.0);
    world.emplace<game::Health>(player, 100.0f, 100.0f);
    world.emplace<engine::Drawable>(player, engine::DrawableFactory::rectangle({0, 0, 1})).shader = &shader;


    // default camera value to see the generated terrain properly
    m_camera.setCenter(glm::vec2(13, 22));
    m_camera.setViewportSize(glm::vec2(109, 64));
}

auto game::ThePurge::onUpdate(entt::registry &world, const engine::Event &e) -> void
{
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
                    break;                                                            // player stop
                case GLFW_KEY_I: movement.publish(world, player, {0.0, 0.1}); break;  // go top
                case GLFW_KEY_K: movement.publish(world, player, {0.0, -0.1}); break; // go bottom
                case GLFW_KEY_L: movement.publish(world, player, {0.1, 0.0}); break;  // go right
                case GLFW_KEY_J: movement.publish(world, player, {-0.1, 0.0}); break; // go left
                case GLFW_KEY_S: castSpell.publish(); break;
                default: return;
                }
            },
            [&](const engine::TimeElapsed &dt) { gameLogic.publish(world, player, dt); },
            [&](auto) {},
        },
        e);


    // todo : update me only if camera updated
    const auto viewProj = m_camera.getViewProjMatrix();
    shader.uploadUniformMat4("viewProj", viewProj);
}

auto game::ThePurge::mapGenerationOverlayTick(entt::registry &world) -> void
{
    static FloorGenParam params;

    ImGui::Begin("MapGeneration");

    if (ImGui::Button("Generate")) {
        // TODO: check how to handle seeding better
        auto data = generateFloor(world, &shader, params, static_cast<std::uint32_t>(std::time(nullptr)));
        auto &pos = world.get<engine::d3::Position>(player);

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

static bool show_demo_window = true;

auto game::ThePurge::drawUserInterface(entt::registry &world) -> void
{
    if (show_demo_window) { ImGui::ShowDemoWindow(&show_demo_window); }

    ImGui::Begin("Camera");

    if (ImGui::Button("Reset")) m_camera = engine::Camera();

    {
        auto cameraPos = m_camera.getCenter();
        ImGui::Text("Camera Position (%.3f, %.3f)", static_cast<double>(cameraPos.x), static_cast<double>(cameraPos.y));

        bool updated = false;
        updated |= ImGui::DragFloat("Camera X", &cameraPos.x);
        updated |= ImGui::DragFloat("Camera Y", &cameraPos.y);

        if (updated) m_camera.setCenter(cameraPos);
    }

    {
        auto viewPortSize = m_camera.getViewportSize();
        const auto pos = m_camera.getCenter();

        ImGui::Text("Viewport size (%.3f, %.3f)", static_cast<double>(viewPortSize.x), static_cast<double>(viewPortSize.y));
        ImGui::Text("Viewport range :");
        ImGui::Text("   left  : %.3f", static_cast<double>(pos.x - (viewPortSize.x / 2)));
        ImGui::Text("   right : %.3f", static_cast<double>(pos.x + (viewPortSize.x / 2)));
        ImGui::Text("   top   : %.3f", static_cast<double>(pos.y + (viewPortSize.y / 2)));
        ImGui::Text("   bottom: %.3f", static_cast<double>(pos.y - (viewPortSize.y / 2)));


        bool updated = false;
        updated |= ImGui::DragFloat("Viewport width", &viewPortSize.x, 1.f, 2.f);
        updated |= ImGui::DragFloat("Viewport height", &viewPortSize.y, 1.f, 2.f);

        if (updated) m_camera.setViewportSize(viewPortSize);
    }


    ImGui::End();

    mapGenerationOverlayTick(world);
}
