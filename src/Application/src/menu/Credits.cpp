#include <Engine/helpers/TextureLoader.hpp>
#include <Engine/Core.hpp>

#include "menu/Credits.hpp"
#include "menu/MainMenu.hpp"
#include "ThePURGE.hpp"

void game::menu::Credits::create(entt::registry &, ThePURGE &) {

    static auto holder = engine::Core::Holder{};

    const auto &dataFolder = holder.instance->settings().data_folder;

    m_texture = engine::helper::loadTexture(dataFolder + "menus/credits.png");
}

void game::menu::Credits::draw(entt::registry &, ThePURGE & game)
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(frac2pixel({1.f, 1.f}));
    ImGui::Begin("Credits", nullptr, ImGuiWindowFlags_NoDecoration);
    drawTexture(m_texture, ImVec2(0, 0), frac2pixel(ImVec2(1, 1)));
    ImGui::End();


    if (select() || m_exit)
        game.setMenu(std::make_unique<menu::MainMenu>());
}

void game::menu::Credits::event(entt::registry &, ThePURGE &, const engine::Event & e)
{
    std::visit(
        engine::overloaded{
            [&](const engine::Pressed<engine::JoystickButton> &joy) {
                switch (joy.source.button) {
                case engine::Joystick::ACTION_RIGHT: m_exit = true; break;
                default: return;
                }
            },
            [&](auto) {},
        },
        e);
}