#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Engine/Graphics/third_party.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Event/Event.hpp>
#include <Engine/audio/AudioManager.hpp>
#include <Engine/Settings.hpp>
#include <Engine/Core.hpp>

#include <Engine/helpers/ImGui.hpp>

#include "level/LevelTilemapBuilder.hpp"
#include "level/MapGenerator.hpp"
#include "factory/EntityFactory.hpp"

#include "factory/EntityFactory.hpp"

#include "GameLogic.hpp"
#include "ThePURGE.hpp"

#include "DataConfigLoader.hpp"

using namespace std::chrono_literals;

game::ThePurge::ThePurge()  {}

auto game::ThePurge::onDestroy(entt::registry &) -> void {}

auto game::ThePurge::onCreate([[maybe_unused]] entt::registry &world) -> void {

    static auto holder = engine::Core::Holder{};

    m_nextFloorSeed = static_cast<std::uint32_t>(std::time(nullptr));
    m_logics = std::make_unique<GameLogic>(*this);
    m_debugConsole = std::make_unique<DebugConsole>(*this);

    m_dungeonMusic =
        holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/dungeon_music.wav");
    m_dungeonMusic->setVolume(0.1f).setLoop(true);

    m_debugConsole->info("Press TAB to autocomplete known commands.\nPress F1 to toggle this console");

    m_classDatabase = DataConfigLoader::loadClassDatabase(holder.instance->settings().data_folder + "config/classes.json");

    setState(State::LOADING);
}

auto game::ThePurge::onUpdate(entt::registry &world, const engine::Event &e) -> void
{
    static auto holder = engine::Core::Holder{};

    if (m_state == State::IN_GAME) {
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
                    case GLFW_KEY_I: m_logics->movement.publish(world, player, {0.0, 0.1}); break;  // go top
                    case GLFW_KEY_K: m_logics->movement.publish(world, player, {0.0, -0.1}); break; // go bottom
                    case GLFW_KEY_L: m_logics->movement.publish(world, player, {0.1, 0.0}); break;  // go right
                    case GLFW_KEY_J: m_logics->movement.publish(world, player, {-0.1, 0.0}); break; // go left
                    case GLFW_KEY_U: {
                        auto &spell = world.get<SpellSlots>(player).spells[0];
                        if (!spell.has_value()) break;

                        auto &vel = world.get<engine::d2::Velocity>(player);
                        m_logics->castSpell.publish(world, player, {vel.x, vel.y}, spell.value());
                        break;
                    }
                    case GLFW_KEY_Y: {
                        auto &spell = world.get<SpellSlots>(player).spells[1];
                        if (!spell.has_value()) break;

                        auto &vel = world.get<engine::d2::Velocity>(player);
                        m_logics->castSpell.publish(world, player, {vel.x, vel.y}, spell.value());
                        break;
                    }
                    default: return;
                    }
                },
                [&](const engine::TimeElapsed &dt) { m_logics->gameUpdated.publish(world, dt); },
                [&](const engine::Moved<engine::JoystickAxis> &joy) {
                    auto joystick = holder.instance->getJoystick(joy.source.id);
                    m_logics->movement.publish(world, player, {((*joystick)->axes[0] / 10.0f), -((*joystick)->axes[1] / 10.0f)});
                },
                [&](auto) {},
            },
            e);

        if (m_camera.isUpdated()) { holder.instance->updateView(m_camera.getViewProjMatrix()); }
    }
}

void game::ThePurge::displaySoundDebugGui()
{
    static auto holder = engine::Core::Holder{};

    static std::vector<std::shared_ptr<engine::Sound>> sounds;

    ImGui::Begin("Sound debug window");

    if (ImGui::Button("Load Music")) {
        try {
            sounds.push_back(holder.instance->getAudioManager().getSound(
                holder.instance->settings().data_folder + "/sounds/dungeon_music.wav"));
        } catch (...) {
        }
    }
    if (ImGui::Button("Load Hit sound")) {
        try {
            sounds.push_back(
                holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "/sounds/hit.wav"));
        } catch (...) {
        }
    }
    ImGui::Separator();

    std::shared_ptr<engine::Sound> toRemove = nullptr;

    int loopId = 0;
    for (const auto &s : sounds) {
        ImGui::PushID(loopId++);

        ImGui::Text("Status :");
        ImGui::SameLine();

        switch (s->getStatus()) {
        case engine::SoundStatus::INITIAL: ImGui::TextColored(ImVec4(1, 1, 1, 1), "Initial"); break;
        case engine::SoundStatus::PLAYING: ImGui::TextColored(ImVec4(0.2f, 1, 0.2f, 1), "Playing"); break;
        case engine::SoundStatus::PAUSED: ImGui::TextColored(ImVec4(1, 1, 0.4f, 1), "Paused"); break;
        case engine::SoundStatus::STOPPED: ImGui::TextColored(ImVec4(1, 0.2f, 0.2f, 1), "Stopped"); break;
        }

        if (ImGui::Button("Play")) s->play();
        if (ImGui::Button("Sop")) s->stop();

        auto speed = s->getSpeed();
        if (ImGui::SliderFloat("Speed", &speed, 0.5, 2)) s->setSpeed(speed);

        auto volume = s->getVolume();
        if (ImGui::SliderFloat("Volume", &volume, 0, 5)) s->setVolume(volume);

        auto loop = s->doesLoop();
        if (ImGui::Checkbox("Loop", &loop)) s->setLoop(loop);


        if (ImGui::Button("Forget")) {
            toRemove = s;
            s->stop();
        }

        ImGui::PopID();

        ImGui::Separator();
    }

    if (toRemove) sounds.erase(std::find(std::begin(sounds), std::end(sounds), toRemove));

    ImGui::End();
}

auto game::ThePurge::mapGenerationOverlayTick(entt::registry &world) -> void
{
    static bool spamNextFloor = false;

    ImGui::Begin("MapGeneration");

    ImGui::Checkbox("Spam next floor", &spamNextFloor);

    if (ImGui::Button("Next floor") || spamNextFloor) m_logics->onFloorChange.publish(world);

    ImGui::SliderInt(
        "Min room size", &m_logics->m_map_generation_params.minRoomSize, 0, m_logics->m_map_generation_params.maxRoomSize);
    ImGui::SliderInt(
        "Max room size", &m_logics->m_map_generation_params.maxRoomSize, m_logics->m_map_generation_params.minRoomSize, 50);
    ImGui::Separator();

    // Assuming std::size_t is uint32_t
    ImGui::InputScalar("Min room count", ImGuiDataType_U32, &m_logics->m_map_generation_params.minRoomCount);
    ImGui::InputScalar("Max room count", ImGuiDataType_U32, &m_logics->m_map_generation_params.maxRoomCount);
    ImGui::Text("note: actual room count may be smaller if there is not enough space");
    ImGui::Separator();

    ImGui::DragInt("Max dungeon width", &m_logics->m_map_generation_params.maxDungeonWidth, 0, 500);
    ImGui::DragInt("Max dungeon height", &m_logics->m_map_generation_params.maxDungeonHeight, 0, 500);
    ImGui::Separator();

    ImGui::SliderInt(
        "Min corridor width",
        &m_logics->m_map_generation_params.minCorridorWidth,
        0,
        m_logics->m_map_generation_params.maxCorridorWidth);
    ImGui::SliderInt(
        "Max corridor width",
        &m_logics->m_map_generation_params.maxCorridorWidth,
        m_logics->m_map_generation_params.minCorridorWidth,
        50);
    ImGui::Separator();

    ImGui::SliderFloat("Enemy per block", &m_logics->m_map_generation_params.mobDensity, 0, 1);

    ImGui::End();
}

auto game::ThePurge::drawUserInterface(entt::registry &world) -> void
{
    [[maybe_unused]] static auto holder = engine::Core::Holder{};

#ifndef NDEBUG
    if (holder.instance->isShowingDebugInfo()) {
        m_debugConsole->draw();
        ImGui::ShowDemoWindow();
    }
#endif

    if (m_state == State::LOADING) {
        // todo : style because this is not a debug window
        ImGui::Begin("Menu loading", nullptr, ImGuiWindowFlags_NoDecoration);

        // note : this block could be launch in a future
        if (ImGui::Button("Start the game")) {
            m_logics->onGameStarted.publish(world);
            setState(State::IN_GAME);
        }

        ImGui::End();
    } else if (m_state == State::IN_GAME) {
        {
            const auto infoHealth = world.get<Health>(player);
            const auto HP = infoHealth.current / infoHealth.max;

            const auto level = world.get<Level>(player);
            const auto Atk = world.get<AttackDamage>(player);
            const auto XP = static_cast<float>(level.current_xp) / static_cast<float>(level.xp_require);
            KeyPicker keyPicker = world.get<KeyPicker>(player);

            // todo : style because this is not a debug window HUD
            ImGui::SetNextWindowPos(ImVec2(m_camera.getViewportSize().x / 10, 10));
            ImGui::SetNextWindowSize(ImVec2(400, 200));
            ImGui::Begin(
                "Info Player",
                nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize
                    | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
            ImGui::ProgressBar(HP, ImVec2(0.f, 0.f), fmt::format("{}/{}", infoHealth.current, infoHealth.max).data());
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Text("HP");
            ImGui::ProgressBar(XP, ImVec2(0.0f, 0.0f));
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Text("XP");
            helper::ImGui::Text("Level: {}", level.current_level);
            helper::ImGui::Text("Speed: {}", 1);
            helper::ImGui::Text("Atk: {}", Atk.damage);

            if (keyPicker.hasKey) helper::ImGui::Text("You have the key");
            ImGui::End();


            mapGenerationOverlayTick(world);
        }
#ifndef NDEBUG
        if (holder.instance->isShowingDebugInfo()) {
            ImGui::Begin("Camera");
            if (ImGui::Button("Reset")) m_camera = engine::Camera();

            auto cameraPos = m_camera.getCenter();
            helper::ImGui::Text("Camera Position ({}, {})", cameraPos.x, cameraPos.y);

            bool pos_updated = false;
            pos_updated |= ImGui::DragFloat("Camera X", &cameraPos.x);
            pos_updated |= ImGui::DragFloat("Camera Y", &cameraPos.y);

            if (pos_updated) m_camera.setCenter(cameraPos);

            auto viewPortSize = m_camera.getViewportSize();
            const auto pos = m_camera.getCenter();

            helper::ImGui::Text("Viewport size ({}, {})", viewPortSize.x, viewPortSize.y);
            ImGui::Text("Viewport range :");
            helper::ImGui::Text("   left  : {}", pos.x - viewPortSize.x / 2.0f);
            helper::ImGui::Text("   right : {}", pos.x + viewPortSize.x / 2.0f);
            helper::ImGui::Text("   top   : {}", pos.y + viewPortSize.y / 2.0f);
            helper::ImGui::Text("   bottom: {}", pos.y - viewPortSize.y / 2.0f);

            bool updated = false;
            updated |= ImGui::DragFloat("Viewport width", &viewPortSize.x, 1.f, 2.f);
            updated |= ImGui::DragFloat("Viewport height", &viewPortSize.y, 1.f, 2.f);

            if (updated) m_camera.setViewportSize(viewPortSize);
            ImGui::End();

            displaySoundDebugGui();
        }
#endif
    } else if (m_state == State::GAME_OVER) {
        // todo : style because this is not a debug window
        ImGui::Begin("Menu Game Over", nullptr, ImGuiWindowFlags_NoDecoration);

        if (ImGui::Button("Your are dead !")) {
            for (const auto &i : world.view<entt::tag<"enemy"_hs>>()) { world.destroy(i); }
            for (const auto &i : world.view<entt::tag<"terrain"_hs>>()) { world.destroy(i); }
            for (const auto &i : world.view<entt::tag<"key"_hs>>()) { world.destroy(i); }
            for (const auto &i : world.view<entt::tag<"player"_hs>>()) { world.destroy(i); }
            for (const auto &i : world.view<entt::tag<"spell"_hs>>()) { world.destroy(i); }

            setState(State::LOADING);
        }

        ImGui::End();
    }
}

auto game::ThePurge::goToNextFloor(entt::registry &world) -> void
{
    world.view<entt::tag<"terrain"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<entt::tag<"enemy"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<entt::tag<"spell"_hs>>().each([&](auto &e) { world.destroy(e); });

    auto data = generateFloor(world, m_map_generation_params, m_nextFloorSeed);
    m_nextFloorSeed = data.nextFloorSeed;

    auto &pos = world.get<engine::d3::Position>(player);

    pos.x = data.spawn.x + data.spawn.w * 0.5;
    pos.y = data.spawn.y + data.spawn.h * 0.5;
}
