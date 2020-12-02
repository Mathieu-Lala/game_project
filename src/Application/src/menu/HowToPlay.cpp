#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/helpers/TextureLoader.hpp>
#include <Engine/Core.hpp>
#include <Engine/api/Core.hpp>

#include "models/Spell.hpp"

#include "menu/HowToPlay.hpp"
#include "menu/MainMenu.hpp"
#include "ThePURGE.hpp"

void game::menu::HowToPlay::create(entt::registry &, ThePURGE &)
{
    auto core = engine::api::getCore();

    const auto &dataFolder = core->settings().data_folder;

    m_howToPlay = engine::helper::loadTexture(dataFolder + "menus/how_to_play/howtoplay.png");
    m_controls = engine::helper::loadTexture(dataFolder + "menus/how_to_play/controls.png");
}

void game::menu::HowToPlay::draw(entt::registry &, ThePURGE &game)
{
    auto core = engine::api::getCore();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(frac2pixel({1.f, 1.f}));
    ImGui::Begin("HowToPlay", nullptr, ImGuiWindowFlags_NoDecoration);

    if (right()) {
        core->getAudioManager().getSound(core->settings().data_folder + "sounds/menu/change.wav")->play();

        m_currentTab = Tab::CONTROLS;
    }
    if (left()) {
        core->getAudioManager().getSound(core->settings().data_folder + "sounds/menu/change.wav")->play();

        m_currentTab = Tab::HOW_TO_PLAY;
    }

    switch (m_currentTab) {
    case Tab::HOW_TO_PLAY: drawTexture(m_howToPlay, ImVec2(0, 0), frac2pixel(ImVec2(1, 1))); break;
    case Tab::CONTROLS: drawTexture(m_controls, ImVec2(0, 0), frac2pixel(ImVec2(1, 1))); break;
    }

    ImGui::End();

    if (close()) {
        core->getAudioManager().getSound(core->settings().data_folder + "sounds/menu/back.wav")->play();

        game.setMenu(std::make_unique<menu::MainMenu>());
    }
}

void game::menu::HowToPlay::event(entt::registry &, ThePURGE &, const engine::Event &e)
{
    std::visit(
        engine::overloaded{
            [&](const engine::Pressed<engine::JoystickButton> &joy) {
                switch (joy.source.button) {
                case engine::Joystick::ACTION_RIGHT: forceClose(true); break;
                case engine::Joystick::ACTION_BOTTOM: forceClose(true); break;
                case engine::Joystick::LS: forceLeft(true); break;
                case engine::Joystick::RS: forceRight(true); break;
                default: break;
                }
            },
            [&](const engine::Pressed<engine::Key> &key) {
                switch (key.source.key) {
                case GLFW_KEY_ENTER: forceClose(true); break;
                default: break;
                }
            },
            [&](auto) {},
        },
        e);
}
