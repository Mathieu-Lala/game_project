#include <Engine/helpers/TextureLoader.hpp>

#include "menu/MainMenu.hpp"
#include "ThePURGE.hpp"

void game::menu::MainMenu::create(entt::registry &, ThePURGE &)
{
    static auto holder = engine::Core::Holder{};

    const auto &dataFolder = holder.instance->settings().data_folder;


    m_backgroundTexture = engine::helper::loadTexture(dataFolder + "menus/main/background.png");



}

void game::menu::MainMenu::draw(entt::registry &, ThePURGE &)
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(frac2pixel({1.f, 1.f}));

    ImGui::Begin("MainMenu", nullptr, ImGuiWindowFlags_NoDecoration);

    //if (ImGui::Button("Start the game")) {
    //    game.logics()->onGameStart.publish(world);
    //    game.setMenu(nullptr);
    //}

    drawTexture(m_backgroundTexture, ImVec2(0, 0), frac2pixel({1.f, 1.f}));

    ImGui::End();
}

void game::menu::MainMenu::event(entt::registry &world, ThePURGE &game, const engine::Event &e)
{
    std::visit(
        engine::overloaded{
            [&](const engine::Pressed<engine::JoystickButton> &joy) {
                switch (joy.source.button) {
                case engine::Joystick::CENTER2: {
                    game.logics()->onGameStart.publish(world);
                    game.setMenu(nullptr);
                } break;
                default: break;
                }
            },
            [&](auto) {},
        },
        e);
}
