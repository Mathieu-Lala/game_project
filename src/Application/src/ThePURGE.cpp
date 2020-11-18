#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>
#include <Engine/Graphics/third_party.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Event/Event.hpp>
#include <Engine/audio/AudioManager.hpp>
#include <Engine/Settings.hpp>
#include <Engine/component/Color.hpp>
#include <Engine/component/Texture.hpp>
#include <Engine/Core.hpp>

#include <Engine/helpers/ImGui.hpp>

#include "level/LevelTilemapBuilder.hpp"
#include "level/MapGenerator.hpp"
#include "factory/EntityFactory.hpp"

#include "factory/EntityFactory.hpp"

#include "screen/MainMenu.hpp"
#include "widgets/UserStatistics.hpp"

#include "GameLogic.hpp"
#include "ThePURGE.hpp"

#include "DataConfigLoader.hpp"

using namespace std::chrono_literals;

game::ThePURGE::ThePURGE() {}

auto game::ThePURGE::onDestroy(entt::registry &) -> void {}

auto game::ThePURGE::onCreate([[maybe_unused]] entt::registry &world) -> void
{
    static auto holder = engine::Core::Holder{};

    m_nextFloorSeed = static_cast<std::uint32_t>(std::time(nullptr));
    m_logics = std::make_unique<GameLogic>(*this);
    m_debugConsole = std::make_unique<DebugConsole>(*this);

    m_dungeonMusic =
        holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/dungeon_music.wav");
    m_dungeonMusic->setVolume(0.1f).setLoop(true);

    m_debugConsole->info("Press TAB to autocomplete known commands.\nPress F1 to toggle this console");

    m_classDatabase = DataConfigLoader::loadClassDatabase(holder.instance->settings().data_folder + "db/classes.json");

    setState(State::LOADING);
}

auto game::ThePURGE::onUpdate(entt::registry &world, const engine::Event &e) -> void
{
    static auto holder = engine::Core::Holder{};
    constexpr auto kDebugKeyboardPlayerMS = 15;

    if (m_state == State::IN_GAME) {
        std::visit(
            engine::overloaded{
                [&](const engine::Pressed<engine::Key> &key) {
                    switch (key.source.key) {
                    case GLFW_KEY_UP: m_camera.move({0, 1}); break;
                    case GLFW_KEY_RIGHT: m_camera.move({1, 0}); break;
                    case GLFW_KEY_DOWN: m_camera.move({0, -1}); break;
                    case GLFW_KEY_LEFT: m_camera.move({-1, 0}); break;

                    case GLFW_KEY_I:
                        world.get<engine::d2::Velocity>(player).y += kDebugKeyboardPlayerMS;
                        break; // go top
                    case GLFW_KEY_K:
                        world.get<engine::d2::Velocity>(player).y -= kDebugKeyboardPlayerMS;
                        break; // go bottom
                    case GLFW_KEY_L:
                        world.get<engine::d2::Velocity>(player).x += kDebugKeyboardPlayerMS;
                        break; // go right
                    case GLFW_KEY_J:
                        world.get<engine::d2::Velocity>(player).x -= kDebugKeyboardPlayerMS;
                        break; // go left

                    case GLFW_KEY_P: setState(State::IN_INVENTORY); break;

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
                [&](const engine::Released<engine::Key> &key) {
                    switch (key.source.key) {
                    case GLFW_KEY_I:
                        world.get<engine::d2::Velocity>(player).y -= kDebugKeyboardPlayerMS;
                        break; // Stop going top
                    case GLFW_KEY_K:
                        world.get<engine::d2::Velocity>(player).y += kDebugKeyboardPlayerMS;
                        break; // Stop going bottom
                    case GLFW_KEY_L:
                        world.get<engine::d2::Velocity>(player).x -= kDebugKeyboardPlayerMS;
                        break; // Stop going right
                    case GLFW_KEY_J:
                        world.get<engine::d2::Velocity>(player).x += kDebugKeyboardPlayerMS;
                        break; // Stop going left
                    default: return;
                    }
                },
                [&](const engine::TimeElapsed &dt) { m_logics->gameUpdated.publish(world, dt); },
                [&](const engine::Moved<engine::JoystickAxis> &joy) {
                    switch (joy.source.axis) {
                        case engine::Joystick::LST: {
                            auto &spell = world.get<SpellSlots>(player).spells[2];
                            if (!spell.has_value()) break;
                            auto &vel = world.get<engine::d2::Velocity>(player);
                            m_logics->castSpell.publish(world, player, {vel.x, vel.y}, spell.value());
                            break;
                        }
                        case engine::Joystick::RST: {
                            auto &spell = world.get<SpellSlots>(player).spells[3];
                            if (!spell.has_value()) break;
                            auto &vel = world.get<engine::d2::Velocity>(player);
                            m_logics->castSpell.publish(world, player, {vel.x, vel.y}, spell.value());
                            break;
                        }
                        default: {
                            auto joystick = holder.instance->getJoystick(joy.source.id);
                            m_logics->movement.publish(
                                world,
                                player,
                                {(static_cast<double>((*joystick)->axes[0]) / 10.0),
                                 -(static_cast<double>((*joystick)->axes[1]) / 10.0)});
                            break;
                        }
                    }
                },
                [&](const engine::Pressed<engine::JoystickButton> &joy) {
                    switch (joy.source.button) {
                    case engine::Joystick::CENTER2: setState(State::IN_INVENTORY); break;
                    case engine::Joystick::LS: {
                        auto &spell = world.get<SpellSlots>(player).spells[0];
                        if (!spell.has_value()) break;

                        auto &vel = world.get<engine::d2::Velocity>(player);
                        m_logics->castSpell.publish(world, player, {vel.x, vel.y}, spell.value());
                        break;
                    }
                    case engine::Joystick::RS: {
                        auto &spell = world.get<SpellSlots>(player).spells[1];
                        if (!spell.has_value()) break;

                        auto &vel = world.get<engine::d2::Velocity>(player);
                        m_logics->castSpell.publish(world, player, {vel.x, vel.y}, spell.value());
                        break;
                    }
                    default: return;
                    }
                },
                [&](auto) {},
            },
            e);
        auto &pos = world.get<engine::d3::Position>(player);
        m_camera.setCenter({pos.x, pos.y});
        if (m_camera.isUpdated()) { holder.instance->updateView(m_camera.getViewProjMatrix()); }
    } else if (m_state == State::IN_INVENTORY) {
        std::visit(
            engine::overloaded{
                [&](const engine::Pressed<engine::Key> &key) {
                    switch (key.source.key) {
                    case GLFW_KEY_P: setState(State::IN_GAME); break;
                    default: return;
                    }
                },
                [&](const engine::Pressed<engine::JoystickButton> &joy) {
                    switch (joy.source.button) {
                    case engine::Joystick::CENTER2: setState(State::IN_GAME); break;
                    }
                },
                [&](auto) {},
            },
            e);
    } else if (m_state == State::GAME_OVER) {
        std::visit(
            engine::overloaded{
                [&](const engine::Pressed<engine::JoystickButton> &joy) {
                    switch (joy.source.button) {
                        case engine::Joystick::CENTER2: {
                            for (const auto &i : world.view<entt::tag<"enemy"_hs>>()) { world.destroy(i); }
                            for (const auto &i : world.view<entt::tag<"terrain"_hs>>()) { world.destroy(i); }
                            for (const auto &i : world.view<entt::tag<"key"_hs>>()) { world.destroy(i); }
                            for (const auto &i : world.view<entt::tag<"player"_hs>>()) { world.destroy(i); }
                            for (const auto &i : world.view<entt::tag<"spell"_hs>>()) { world.destroy(i); }

                            setState(State::LOADING);
                        }
                    }
                },
                [&](auto) {},
            },
            e);
    } else if (m_state == State::LOADING) {
        std::visit(
            engine::overloaded{
                [&](const engine::Pressed<engine::JoystickButton> &joy) {
                    switch (joy.source.button) {
                    case engine::Joystick::CENTER2: {
                        logics()->onGameStarted.publish(world);
                        setState(ThePURGE::State::IN_GAME);
                    }
                    }
                },
                [&](auto) {},
            },
            e);
    }
}


void game::ThePURGE::displaySoundDebugGui()
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

auto game::ThePURGE::mapGenerationOverlayTick(entt::registry &world) -> void
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

    ImGui::InputScalar("Min room count", ImGuiDataType_U64, &m_logics->m_map_generation_params.minRoomCount);
    ImGui::InputScalar("Max room count", ImGuiDataType_U64, &m_logics->m_map_generation_params.maxRoomCount);
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

auto game::ThePURGE::drawUserInterface(entt::registry &world) -> void
{
    [[maybe_unused]] static auto holder = engine::Core::Holder{};

#ifndef NDEBUG
    if (holder.instance->isShowingDebugInfo()) {
        m_debugConsole->draw();
        ImGui::ShowDemoWindow();
    }
#endif

    if (m_state == State::LOADING) {
        MainMenu::draw(*this, world);

    } else if (m_state == State::IN_GAME) {
        UserStatistics::draw(*this, world);
        

#ifndef NDEBUG
        if (holder.instance->isShowingDebugInfo()) {
            mapGenerationOverlayTick(world);

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
    } else if (m_state == State::IN_INVENTORY) {
        UserStatistics::draw(*this, world);
        {
            const auto &boughtClasses = world.get<Classes>(player).ids;
            const auto skillPoints = world.get<SkillPoint>(player).count;

            // const auto comp = world.get<Class>(player);
            static bool choosetrigger = false;
            static auto test = classes::getStarterClass(m_classDatabase);
            static std::optional<Class> selectedClass;
            static int spellmapped = 0;
            static int infoAdd;
            static std::string chosenTrig = "";
            static std::vector<GLuint> Texture = {
                engine::DrawableFactory::createtexture(std::string("data/textures/InfoHud.png")),
                engine::DrawableFactory::createtexture(std::string("data/textures/CPoint.PNG")),
                engine::DrawableFactory::createtexture(std::string("data/textures/validate.png")),
                engine::DrawableFactory::createtexture(std::string("data/textures/controller/LB.png")),
                engine::DrawableFactory::createtexture(std::string("data/textures/controller/LT.png")),
                engine::DrawableFactory::createtexture(std::string("data/textures/controller/RT.png")),
                engine::DrawableFactory::createtexture(std::string("data/textures/controller/RB.png")),
            };
            ImGui::SetNextWindowPos(ImVec2(m_camera.getCenter().x + 500.0f, 0.0f));
            ImVec2 size = ImVec2(1000.0f, 1000.0f);
            ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
            ImGui::Begin(
                "Evolution Panel",
                nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize
                    | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
            size = ImGui::GetWindowSize();
            ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>((Texture[0]))), ImVec2(size.x - 30, size.y - 10));
            ImGui::SetCursorPos(ImVec2(0, 200));
            ImVec2 next;
            if (selectedClass.has_value()) {
                ImVec2 icon = ImVec2(ImGui::GetCursorPosX() + size.x / 3 + 9, ImGui::GetCursorPosY());
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3 + 9, ImGui::GetCursorPosY()));
                helper::ImGui::Text("Class Name: {}", selectedClass->name);
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY()));
                helper::ImGui::Text("Class description: {}", selectedClass->description);
                if (infoAdd == 1) {
                    static int triggerValue = 5;
                    std::optional<Classes> lastclass = world.get<Classes>(player);
                    if (spellmapped % 2 != 0 && lastclass.value().ids.size() > 1
                        && lastclass.value().ids[lastclass.value().ids.size() - 1] == selectedClass.value().id) {
                        ImGui::SetCursorPos(ImVec2(
                            ImGui::GetCursorPosX() + size.x / 3,
                            ImGui::GetCursorPosY() + 50));
                        ImGui::Text("Choose Your trigger :");
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY() + 10));
                        auto &spell = world.get<SpellSlots>(player);
                        if (ImGui::ImageButton(
                                reinterpret_cast<void *>(static_cast<intptr_t>((Texture[3]))), ImVec2(50, 50))) {
                            chosenTrig = "LB";
                            triggerValue = 0;
                        }
                        ImGui::SameLine();
                        if (ImGui::ImageButton(
                                reinterpret_cast<void *>(static_cast<intptr_t>((Texture[4]))), ImVec2(50, 50))) {
                            chosenTrig = "LT";
                            triggerValue = 2;
                        }
                        ImGui::SameLine();
                        if (ImGui::ImageButton(
                                reinterpret_cast<void *>(static_cast<intptr_t>((Texture[5]))), ImVec2(50, 50))) {
                            chosenTrig = "RT";
                            triggerValue = 3;
                        }
                        ImGui::SameLine();
                        if (ImGui::ImageButton(
                                reinterpret_cast<void *>(static_cast<intptr_t>((Texture[6]))), ImVec2(50, 50))) {
                            chosenTrig = "RB";
                            triggerValue = 1;
                        }
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY() + 10));
                        helper::ImGui::Text("You choosed the trigger: {}", chosenTrig.c_str());
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY()));
                        if (chosenTrig != "" ) {
                            if (ImGui::Button("Validate")) {
                                choosetrigger = false;
                                auto NewSpell = Spell::create(selectedClass.value().spells[0]);
                                for (int i = 0; i < spell.spells.size(); i++) { 
                                    if (spell.spells[i].has_value() && NewSpell.id == spell.spells[i].value().id) {
                                        spell.removeElem(i);
                                    } 
                                }
                                spell.spells[triggerValue] = NewSpell;
                                spellmapped++;
                            }
                        }
                    } else {
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY()));
                        ImGui::Text("Already bought");
                    }
                    next = ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
                    ImGui::SetCursorPos(ImVec2(icon.x - 50, icon.y));
                    ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>((Texture[2]))), ImVec2(50, 50));
                } else if (infoAdd == 2) {
                    if (skillPoints > 0) {
                        if (choosetrigger == true) {
                            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY()));
                            ImGui::Text("You haven't choose a trigger for the spell");
                        }
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY()));
                        if (ImGui::Button("Add class")) {
                            m_logics->onPlayerBuyClass.publish(world, player, selectedClass.value());
                            infoAdd = 1;
                            if (choosetrigger == false) {
                                spellmapped++;
                                choosetrigger = true;
                            } else
                                spellmapped = spellmapped + 2;
                        }
                    } else {
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY() + 10));
                        helper::ImGui::Button("No skill point", ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
                    }

                } else {
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY()));
                    ImGui::Text("You can't buy it yet");
                }
            }
            // Competences
            ImGui::SetCursorPos(ImVec2(size.x / 2 - 17 * ImGui::GetFontSize() / 2, size.y / 2));
            helper::ImGui::Text("Point de comp: {}", skillPoints); // rendre dynamique le nombre de point de comp
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - 5));
            ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>((Texture[1]))), ImVec2(20, 20));

            // Classes tree
            float length = size.x / 2 - (12.0f + static_cast<float>(test.name.length())) * ImGui::GetFontSize();
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + length, ImGui::GetCursorPosY() + 30));
            helper::ImGui::Text("class Name: {}", test.name);

            std::vector<Class::ID> currentLine;
            std::vector<Class::ID> nextLine;
            std::vector<Class::ID> buyableClasses;
            int tier = 0;

            nextLine.push_back(classes::getStarterClass(m_classDatabase).id);

            while (!nextLine.empty()) {
                ++tier;
                currentLine.clear();
                std::swap(currentLine, nextLine);
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 4, ImGui::GetCursorPosY() + 10));
                helper::ImGui::Text("Tier : {}", tier);
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 4, ImGui::GetCursorPosY() + 10));
                for (const auto currentId : currentLine) {
                    const auto &currentClass = m_classDatabase[currentId];

                    bool bought = std::find(boughtClasses.begin(), boughtClasses.end(), currentId) != boughtClasses.end();
                    bool buyable =
                        std::find(buyableClasses.begin(), buyableClasses.end(), currentId) != buyableClasses.end();
                    if (bought) {
                        buyableClasses.insert(
                            buyableClasses.end(), currentClass.childrenClass.begin(), currentClass.childrenClass.end());
                        if (helper::ImGui::Button(currentClass.name.c_str(), ImVec4(0, 1, 0, 0.5))) {
                            selectedClass = currentClass;
                            infoAdd = 1;
                        }
                    } else if (buyable) {
                        if (helper::ImGui::Button(currentClass.name.c_str(), ImVec4(1, 1, 0, 0.5))) {
                            selectedClass = currentClass;
                            infoAdd = 2;
                        }
                    } else {
                        if (helper::ImGui::Button(currentClass.name.c_str(), ImVec4(1, 0, 0, 0.5))) {
                            selectedClass = currentClass;
                            infoAdd = 3;
                        }
                    }
                    ImGui::SameLine();
                    nextLine.insert(nextLine.end(), currentClass.childrenClass.begin(), currentClass.childrenClass.end());
                }
                ImGui::Text(" "); // ImGui::NextLine()
            }
            ImGui::End();
        }
    }
}

auto game::ThePURGE::goToNextFloor(entt::registry &world) -> void
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
