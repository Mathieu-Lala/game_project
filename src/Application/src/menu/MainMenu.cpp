#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>

#include "menu/MainMenu.hpp"
#include "ThePURGE.hpp"

bool game::menu::MainMenu::draw(entt::registry &world, ThePURGE &game)
{
    auto ret = true;
    // todo : load the resource in a coroutine here

    // todo : style because this is not a debug window
    ImGui::Begin("Menu loading", nullptr, ImGuiWindowFlags_NoDecoration);

    if (ImGui::Button("Start the game")) {
        game.logics()->onGameStart.publish(world);
        game.setMenu(nullptr);
        ret = false;
    }

    ImGui::End();
    return ret;
}

bool game::menu::MainMenu::event(entt::registry &world, ThePURGE &game, const engine::Event &e)
{
    auto ret = true;
    std::visit(
        engine::overloaded{
            [&](const engine::Pressed<engine::JoystickButton> &joy) {
                switch (joy.source.button) {
                case engine::Joystick::CENTER2: {
                    game.logics()->onGameStart.publish(world);
                    game.setMenu(nullptr);
                    ret = false;
                } break;
                default: break;
                }
            },
            [&](auto) {},
        },
        e);
    return ret;
}
