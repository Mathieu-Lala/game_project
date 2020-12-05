#include <Engine/Core.hpp>
#include <Engine/Graphics/Window.hpp>
#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>

#include "models/Spell.hpp"

#include "component/all.hpp"

#include "ThePURGE.hpp"
#include "menu/GameOver.hpp"
#include "menu/MainMenu.hpp"

auto game::menu::GameOver::clean_world(entt::registry &world) -> void
{
    for (const auto &i : world.view<entt::tag<"enemy"_hs>>()) { world.destroy(i); }
    for (const auto &i : world.view<entt::tag<"terrain"_hs>>()) { world.destroy(i); }
    for (const auto &i : world.view<entt::tag<"key"_hs>>()) { world.destroy(i); }
    for (const auto &i : world.view<entt::tag<"player"_hs>>()) {
        world.destroy(i);
        world.destroy(world.get<AimSight>(i).entity);
    }
    for (const auto &i : world.view<entt::tag<"spell"_hs>>()) { world.destroy(i); }
}

void game::menu::GameOver::create(entt::registry &, ThePURGE &)
{
    engine::Core::Holder{}.instance->window()->setCursorVisible(true);
}

game::menu::GameOver::~GameOver() { engine::Core::Holder{}.instance->window()->setCursorVisible(false); }

void game::menu::GameOver::draw(entt::registry &world, ThePURGE &game)
{
    // todo : style because this is not a debug window
    ImGui::Begin("Menu Game Over", nullptr, ImGuiWindowFlags_NoDecoration);

    ImGui::Text("You are dead !");
    if (ImGui::Button("main menu")) {
        clean_world(world);
        game.setMenu(std::make_unique<menu::MainMenu>());
        game.setBackgroundMusic("sounds/menu/background_music.wav", 0.5f);
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
                    clean_world(world);
                    game.setMenu(std::make_unique<MainMenu>());
                } break;
                default: break;
                }
            },
            [&](auto) {},
        },
        e);
}
