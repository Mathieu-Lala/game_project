#include <string>

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
#include <Engine/component/Spritesheet.hpp>
#include <Engine/Core.hpp>

#include <Engine/helpers/ImGui.hpp>

#include "level/LevelTilemapBuilder.hpp"
#include "level/MapGenerator.hpp"

#include "factory/EntityFactory.hpp"

#include "screen/MainMenu.hpp"
#include "screen/GameOverMenu.hpp"

#include "widgets/UserStatistics.hpp"
#include "widgets/DebugTerrainGeneration.hpp"
#include "widgets/DebugCamera.hpp"
#include "widgets/DebugAudio.hpp"

#include "GameLogic.hpp"
#include "ThePURGE.hpp"

#include "DataConfigLoader.hpp"

#include "component/Facing.hpp"

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

    // pos and size based of `FloorGenParam::maxDungeonWidth / Height`
    EntityFactory::create<EntityFactory::ID::BACKGROUND>(world, glm::vec2(25, 25), glm::vec2(75, 75));

    setState(State::LOADING);
}

auto game::ThePURGE::onUpdate(entt::registry &world, const engine::Event &e) -> void
{
    static auto holder = engine::Core::Holder{};
    constexpr auto kDebugKeyboardPlayerMS = 15;

    const auto spell_map = []<typename T>(T k)
    {
        struct SpellMap {
            int key;
            std::size_t id;
        };
        if constexpr (std::is_same<T, engine::Joystick::Buttons>::value) {
            const auto map = std::to_array<SpellMap>({{engine::Joystick::LS, 0}, {engine::Joystick::RS, 1}});
            return std::find_if(map.begin(), map.end(), [&k](auto &i) { return i.key == k; })->id;

        } else if constexpr (std::is_same<T, engine::Joystick::Axis>::value) {
            const auto map = std::to_array<SpellMap>({{engine::Joystick::LST, 2}, {engine::Joystick::RST, 3}});
            return std::find_if(map.begin(), map.end(), [&k](auto &i) { return i.key == k; })->id;

        } else { // todo : should be engine::Keyboard::Key
            const auto map = std::to_array<SpellMap>({{GLFW_KEY_U, 0}, {GLFW_KEY_Y, 1}, {GLFW_KEY_T, 2}, {GLFW_KEY_R, 3}});
            return std::find_if(map.begin(), map.end(), [&k](auto &i) { return i.key == k; })->id;
        }
    };

    if (m_state == State::IN_GAME) {
        std::visit(
            engine::overloaded{
                [&](const engine::Pressed<engine::Key> &key) {
                    switch (key.source.key) {
                    case GLFW_KEY_UP: m_camera.move({0, 1}); break;
                    case GLFW_KEY_RIGHT: m_camera.move({1, 0}); break;
                    case GLFW_KEY_DOWN: m_camera.move({0, -1}); break;
                    case GLFW_KEY_LEFT: m_camera.move({-1, 0}); break;

                    case GLFW_KEY_I: m_logics->movement.publish(world, player, Direction::UP); break;    // go top
                    case GLFW_KEY_K: m_logics->movement.publish(world, player, Direction::DOWN); break;  // go bottom
                    case GLFW_KEY_L: m_logics->movement.publish(world, player, Direction::RIGHT); break; // go right
                    case GLFW_KEY_J: m_logics->movement.publish(world, player, Direction::LEFT); break;  // go left
                    case GLFW_KEY_P: setState(State::IN_INVENTORY); break;

                    case GLFW_KEY_U:
                    case GLFW_KEY_Y: {
                        const auto id = spell_map(key.source.key);

                        auto &spell = world.get<SpellSlots>(player).spells[id];
                        if (!spell.has_value()) break;

                        auto &vel = world.get<engine::d2::Velocity>(player);
                        m_logics->castSpell.publish(world, player, {vel.x, vel.y}, spell.value());
                    } break;
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
                    case engine::Joystick::LST:
                    case engine::Joystick::RST: {
                        const auto id = spell_map(joy.source.axis);
                        auto &spell = world.get<SpellSlots>(player).spells[id];
                        if (!spell.has_value()) break;
                        auto &vel = world.get<engine::d2::Velocity>(player);
                        m_logics->castSpell.publish(world, player, {vel.x, vel.y}, spell.value());
                    } break;
                    case engine::Joystick::LSX:
                    case engine::Joystick::LSY: {
                        auto joystick = holder.instance->getJoystick(joy.source.id);
                        m_logics->joystickMovement.publish(
                            world,
                            player,
                            {(static_cast<double>((*joystick)->axes[engine::Joystick::LSX]) / 10.0),
                             -(static_cast<double>((*joystick)->axes[engine::Joystick::LSY]) / 10.0)});
                    } break;
                    case engine::Joystick::Axis::RSX:
                    case engine::Joystick::Axis::RSY: { // todo : cleaner
                        auto joystick = holder.instance->getJoystick(joy.source.id);

                        if ((*joystick)->axes[engine::Joystick::Axis::RSX] >= 0) {
                            auto &spell = world.get<SpellSlots>(player).spells[2];
                            if (!spell.has_value()) return;
                            auto &vel = world.get<engine::d2::Velocity>(player);
                            m_logics->castSpell.publish(world, player, {vel.x, vel.y}, spell.value());
                        }
                        if ((*joystick)->axes[engine::Joystick::Axis::RSY] >= 0) {
                            auto &spell = world.get<SpellSlots>(player).spells[3];
                            if (!spell.has_value()) return;
                            auto &vel = world.get<engine::d2::Velocity>(player);
                            m_logics->castSpell.publish(world, player, {vel.x, vel.y}, spell.value());
                        }
                    } break;
                    default: break;
                    }
                },
                [&](const engine::Pressed<engine::JoystickButton> &joy) {
                    switch (joy.source.button) {
                    case engine::Joystick::CENTER2: setState(State::IN_INVENTORY); break;
                    case engine::Joystick::LS:
                    case engine::Joystick::RS: {
                        const auto id = spell_map(joy.source.button);
                        auto &spell = world.get<SpellSlots>(player).spells[id];
                        if (!spell.has_value()) break;
                        auto &vel = world.get<engine::d2::Velocity>(player);
                        m_logics->castSpell.publish(world, player, {vel.x, vel.y}, spell.value());
                    } break;
                    default: return;
                    }
                },
                [&](auto) {},
            },
            e);

        auto &pos = world.get<engine::d3::Position>(player);
        m_camera.setCenter({pos.x, pos.y});
        if (m_camera.isUpdated()) holder.instance->updateView(m_camera.getViewProjMatrix());

    } else if (m_state == State::IN_INVENTORY) {
        std::visit(
            engine::overloaded{
                [&](const engine::Pressed<engine::Key> &key) {
                    switch (key.source.key) {
                    case GLFW_KEY_P: setState(State::IN_GAME); break;
                    default: break;
                    }
                },
                [&](const engine::Pressed<engine::JoystickButton> &joy) {
                    switch (joy.source.button) {
                    case engine::Joystick::CENTER2: setState(State::IN_GAME); break;
                    default: break;
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
                    } break;
                    default: break;
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
                    } break;
                    default: break;
                    }
                },
                [&](auto) {},
            },
            e);
    }
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
            widget::TerrainGeneration::draw(*this, world);
            widget::DebugCamera::draw(m_camera);
            widget::DebugAudio::draw();

            //displaySoundDebugGui();
        }
#endif
    } else if (m_state == State::GAME_OVER) {
        GameOverMenu::draw(*this, world);

    } else if (m_state == State::IN_INVENTORY) { // todo : cleaner
        UserStatistics::draw(*this, world);

        const auto &boughtClasses = world.get<Classes>(player).ids;
        const auto skillPoints = world.get<SkillPoint>(player).count;

        // const auto comp = world.get<Class>(player);
        static bool choosetrigger = false;
        static auto test = classes::getStarterClass(m_classDatabase);
        static std::optional<Class> selectedClass;
        static std::string chosenTrig = "";
        static int spellmapped = 0;
        static int infoAdd;
        // clang-format off
        static std::vector<GLuint> Texture = {
            engine::DrawableFactory::createtexture("data/textures/InfoHud.png"),
            engine::DrawableFactory::createtexture("data/textures/CPoint.PNG"),
            engine::DrawableFactory::createtexture("data/textures/validate.png"),
            engine::DrawableFactory::createtexture("data/textures/controller/LB.png"),
            engine::DrawableFactory::createtexture("data/textures/controller/LT.png"),
            engine::DrawableFactory::createtexture("data/textures/controller/RT.png"),
            engine::DrawableFactory::createtexture("data/textures/controller/RB.png")
        };
        // clang-format on

        ImVec2 size = ImVec2(1000.0f, 1000.0f);
        ImGui::SetNextWindowPos(ImVec2(m_camera.getCenter().x + size.x / 2, 0));
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
                static std::size_t triggerValue = 5;
                std::optional<Classes> lastclass = world.get<Classes>(player);
                if (spellmapped % 2 != 0 && lastclass.value().ids.size() > 1
                    && lastclass.value().ids[lastclass.value().ids.size() - 1] == selectedClass.value().id) {
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY() + 50));
                    ImGui::Text("Choose Your trigger :");
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY() + 10));
                    auto &spell = world.get<SpellSlots>(player);
                    if (ImGui::ImageButton(reinterpret_cast<void *>(static_cast<intptr_t>((Texture[3]))), ImVec2(50, 50))) {
                        chosenTrig = "LB";
                        triggerValue = 0;
                    }
                    ImGui::SameLine();
                    if (ImGui::ImageButton(reinterpret_cast<void *>(static_cast<intptr_t>((Texture[4]))), ImVec2(50, 50))) {
                        chosenTrig = "LT";
                        triggerValue = 2;
                    }
                    ImGui::SameLine();
                    if (ImGui::ImageButton(reinterpret_cast<void *>(static_cast<intptr_t>((Texture[5]))), ImVec2(50, 50))) {
                        chosenTrig = "RT";
                        triggerValue = 3;
                    }
                    ImGui::SameLine();
                    if (ImGui::ImageButton(reinterpret_cast<void *>(static_cast<intptr_t>((Texture[6]))), ImVec2(50, 50))) {
                        chosenTrig = "RB";
                        triggerValue = 1;
                    }
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY() + 10));
                    helper::ImGui::Text("You choosed the trigger: {}", chosenTrig.c_str());
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + size.x / 3, ImGui::GetCursorPosY()));
                    if (chosenTrig != "") {
                        if (ImGui::Button("Validate")) {
                            choosetrigger = false;
                            auto NewSpell = Spell::create(selectedClass.value().spells[0]);
                            for (auto i = 0ul; i < spell.spells.size(); i++) {
                                if (spell.spells[i].has_value() && NewSpell.id == spell.spells[i].value().id) {
                                    spell.spells[i] = {};
                                    // spell.removeElem(i);
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

        std::vector<EntityFactory::ID> currentLine;
        std::vector<EntityFactory::ID> nextLine;
        std::vector<EntityFactory::ID> buyableClasses;
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
                bool buyable = std::find(buyableClasses.begin(), buyableClasses.end(), currentId) != buyableClasses.end();
                if (bought) {
                    buyableClasses.insert(
                        buyableClasses.end(), currentClass.children.begin(), currentClass.children.end());
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
                nextLine.insert(nextLine.end(), currentClass.children.begin(), currentClass.children.end());
            }
            ImGui::Text(" "); // ImGui::NextLine()
        }
        ImGui::End();
    }
}

/*
auto game::ThePURGE::goToNextFloor(entt::registry &world) -> void
{
    world.view<entt::tag<"terrain"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<entt::tag<"enemy"_hs>>().each([&](auto &e) { world.destroy(e); });
    world.view<entt::tag<"spell"_hs>>().each([&](auto &e) { world.destroy(e); });

    const auto map = generateFloor(world, m_map_generation_params, m_nextFloorSeed);
    m_nextFloorSeed = map.nextFloorSeed;

    auto &pos = world.get<engine::d3::Position>(player);

    pos.x = map.spawn.x + map.spawn.w * 0.5;
    pos.y = map.spawn.y + map.spawn.h * 0.5;
}
*/
