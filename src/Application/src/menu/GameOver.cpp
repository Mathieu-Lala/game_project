#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>

#include "ThePURGE.hpp"
#include "menu/GameOver.hpp"
#include "menu/MainMenu.hpp"

void game::menu::GameOver::draw(entt::registry &world, ThePURGE &game)
{
    // todo : style because this is not a debug window
    ImGui::Begin("Menu Game Over", nullptr, ImGuiWindowFlags_NoDecoration);

    ImGui::Text("You are dead !");
    if (ImGui::Button("main menu")) {
        for (const auto &i : world.view<entt::tag<"enemy"_hs>>()) { world.destroy(i); }
        for (const auto &i : world.view<entt::tag<"terrain"_hs>>()) { world.destroy(i); }
        for (const auto &i : world.view<entt::tag<"key"_hs>>()) { world.destroy(i); }
        for (const auto &i : world.view<entt::tag<"player"_hs>>()) { world.destroy(i); }
        for (const auto &i : world.view<entt::tag<"spell"_hs>>()) { world.destroy(i); }

        game.setMenu(std::make_unique<menu::MainMenu>());
    }

    ImGui::End();
}

void game::menu::GameOver::event(entt::registry &world, ThePURGE &game, const engine::Event &e)
{
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

                    game.setMenu(std::make_unique<MainMenu>());
                } break;
                default: break;
                }
            },
            [&](auto) {},
        },
        e);
}
