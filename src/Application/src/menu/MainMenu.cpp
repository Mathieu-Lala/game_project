#include <Engine/helpers/TextureLoader.hpp>
#include <Engine/Core.hpp>

#include "menu/MainMenu.hpp"
#include "ThePURGE.hpp"

void game::menu::MainMenu::create(entt::registry &, ThePURGE &)
{
    static auto holder = engine::Core::Holder{};

    const auto &dataFolder = holder.instance->settings().data_folder;


    m_backgroundTexture = engine::helper::loadTexture(dataFolder + "menus/main/background.png");

    // clang-format off

    #define TEXTURE(path) engine::helper::loadTexture(dataFolder + path)
    #define FROM_1080p(x, y) ImVec2(x.0 / 1920.0, y.0 / 1080.0)

    // SAME ORDER AS `Button` ENUM
    m_buttons.emplace_back(MenuTexture{
        .id =       TEXTURE("menus/main/btn_play_selected.png"),
        .topleft =  FROM_1080p(1238, 259),
        .size =     FROM_1080p(229, 155)
    });
        m_buttons.emplace_back(MenuTexture{
        .id =       TEXTURE("menus/main/btn_rules_selected.png"),
        .topleft =  FROM_1080p(1218, 442),
        .size =     FROM_1080p(284, 149)
    });
    m_buttons.emplace_back(MenuTexture{
        .id =       TEXTURE("menus/main/btn_credits_selected.png"),
        .topleft =  FROM_1080p(1195, 629),
        .size =     FROM_1080p(345, 149)
    });
    m_buttons.emplace_back(MenuTexture{
        .id =       TEXTURE("menus/main/btn_exit_selected.png"),
        .topleft =  FROM_1080p(1263, 820),
        .size =     FROM_1080p(195, 149)
    });

    #undef TEXTURE
    #undef FROM_1080p

    // clang-format on
}

void game::menu::MainMenu::draw(entt::registry &world, ThePURGE &game)
{
    static auto holder = engine::Core::Holder{};

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(frac2pixel({1.f, 1.f}));

    ImGui::Begin("MainMenu", nullptr, ImGuiWindowFlags_NoDecoration);

    drawTexture(m_backgroundTexture, ImVec2(0, 0), frac2pixel({1.f, 1.f}));

    if (up() && m_selected > 0) m_selected--;
    if (down() && m_selected < Button::MAX - 1) m_selected++;

    drawTexture(m_buttons.at(m_selected));

    if (select()) {
        switch (m_selected) {
        case Button::PLAY:
            game.setMenu(nullptr);
            game.logics()->onGameStart.publish(world);
            break;
        case Button::RULES:
            // TODO: rules menu
            spdlog::info("rules");
            break;
        case Button::CREDITS:
            // TODO: credits menu
            spdlog::info("credits");
            break;
        case Button::EXIT:
            holder.instance->close();
            break;
        }
    }

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
