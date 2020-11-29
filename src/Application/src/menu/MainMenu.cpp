#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/helpers/TextureLoader.hpp>
#include <Engine/Core.hpp>

#include "models/Spell.hpp"

#include "menu/MainMenu.hpp"
#include "menu/Credits.hpp"
#include "menu/HowToPlay.hpp"
#include "ThePURGE.hpp"

void game::menu::MainMenu::create(entt::registry &, ThePURGE &)
{
    static auto holder = engine::Core::Holder{};

    const auto &dataFolder = holder.instance->settings().data_folder;


    m_backgroundTexture = engine::helper::loadTexture(dataFolder + "menus/main/mainmenu.png");

    // clang-format off

#define TEXTURE(path) engine::helper::loadTexture(dataFolder + path)
#define FROM_1080p(x, y) ImVec2(x / 1920.0f, y / 1080.0f)

    // SAME ORDER AS `Button` ENUM
    m_buttons.emplace_back(
        TEXTURE("menus/main/btn_play_selected.png"),
        FROM_1080p(1238, 259),
        FROM_1080p(229, 155)
    );
    m_buttons.emplace_back(
        TEXTURE("menus/main/btn_howtoplay_selected.png"),
        FROM_1080p(1062, 441),
        FROM_1080p(590, 155)
    );
    m_buttons.emplace_back(
        TEXTURE("menus/main/btn_credits_selected.png"),
        FROM_1080p(1195, 629),
        FROM_1080p(345, 149)
    );
    m_buttons.emplace_back(
        TEXTURE("menus/main/btn_exit_selected.png"),
        FROM_1080p(1263, 820),
        FROM_1080p(195, 149)
    );

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

    if (up() && m_selected > 0) {
        holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/menu/change.wav")->play();

        m_selected--;
    }
    if (down() && m_selected < Button::MAX - 1) {
        holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/menu/change.wav")->play();

        m_selected++;
    }

    drawTexture(m_buttons.at(static_cast<std::size_t>(m_selected)));

    ImGui::End();

    if (select()) {
        holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/menu/accept.wav")->play();

        switch (m_selected) {
        case Button::PLAY:
            game.logics()->onGameStart.publish(world);
            game.setMenu(nullptr);
            return;
        case Button::HOWTOPLAY: game.setMenu(std::make_unique<menu::HowToPlay>()); return;
        case Button::CREDITS: game.setMenu(std::make_unique<menu::Credits>()); return;
        case Button::EXIT: holder.instance->close(); break;
        }
    }
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
