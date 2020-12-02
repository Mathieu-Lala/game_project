#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/helpers/TextureLoader.hpp>
#include <Engine/Core.hpp>
#include <Engine/api/Core.hpp>

#include "models/Spell.hpp"

#include "menu/Credits.hpp"
#include "menu/MainMenu.hpp"
#include "ThePURGE.hpp"

void game::menu::Credits::create(entt::registry &, ThePURGE &)
{
    auto core = engine::api::getCore();

    const auto &dataFolder = core->settings().data_folder;

    m_texture = engine::helper::loadTexture(dataFolder + "menus/credits.png");
}

void game::menu::Credits::draw(entt::registry &, ThePURGE &game)
{
    auto core = engine::api::getCore();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(frac2pixel({1.f, 1.f}));
    ImGui::Begin("Credits", nullptr, ImGuiWindowFlags_NoDecoration);
    drawTexture(m_texture, ImVec2(0, 0), frac2pixel(ImVec2(1, 1)));
    ImGui::End();


    if (close()) {
        core->getAudioManager().getSound(core->settings().data_folder + "sounds/menu/back.wav")->play();

        game.setMenu(std::make_unique<menu::MainMenu>());
    }
}

void game::menu::Credits::event(entt::registry &, ThePURGE &, const engine::Event &e)
{
    std::visit(
        engine::overloaded{
            [&](const engine::Pressed<engine::JoystickButton> &joy) {
                switch (joy.source.button) {
                case engine::Joystick::ACTION_RIGHT: forceClose(true); break;
                case engine::Joystick::ACTION_BOTTOM: forceClose(true); break;
                default: return;
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
