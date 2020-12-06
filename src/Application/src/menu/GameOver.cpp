#include <Engine/Graphics/Window.hpp>
#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/helpers/TextureLoader.hpp>
#include <Engine/Core.hpp>

#include "models/Spell.hpp"

#include "component/all.hpp"

#include "ThePURGE.hpp"
#include "menu/GameOver.hpp"
#include "menu/MainMenu.hpp"

#include "widgets/Fonts.hpp"

auto game::menu::GameOver::clean_world(entt::registry &world) -> void { Stage{}.clear(world, true); }

void game::menu::GameOver::create(entt::registry &, ThePURGE &)
{
    static auto holder = engine::Core::Holder{};

    const auto &dataFolder = holder.instance->settings().data_folder;


    m_backgroundTexture = engine::helper::loadTexture(dataFolder + "menus/game_over/background.png");

    // clang-format off

    // SAME ORDER AS `Button` ENUM
    m_buttons.emplace_back(GUITexture{
        helper::getTexture("menus/game_over/btn_playagain_selected.png"),
        helper::from1080p(701, 665),
        helper::from1080p(515, 156)
    });
    m_buttons.emplace_back(GUITexture{
        helper::getTexture("menus/game_over/btn_menu_selected.png"),
        helper::from1080p(822, 884),
        helper::from1080p(276, 149)
    });
    // clang-format on
}

void game::menu::GameOver::draw(entt::registry &world, ThePURGE &game)
{
    static auto holder = engine::Core::Holder{};

    ImGui::SetNextWindowPos(ImVec2(-5, -1));
    const auto winSize = helper::frac2pixel({1.f, 1.f});
    ImGui::SetNextWindowSize(ImVec2(winSize.x + 10, winSize.y + 6));

    ImGui::SetNextWindowBgAlpha(0.f);

    ImGui::Begin("GameOver", nullptr, ImGuiWindowFlags_NoDecoration);

    helper::drawTexture(
        m_backgroundTexture,
        ImVec2(0, 0),
        helper::frac2pixel({1.f, 1.f}),
        ImVec4(1, 1, 1, static_cast<float>(std::clamp(m_timeElapsed, 0.0, 1.0))));

    if (up() && m_selected > 0) {
        holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/menu/change.wav")->play();

        m_selected--;
    }
    if (down() && m_selected < Button::MAX - 1) {
        holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/menu/change.wav")->play();

        m_selected++;
    }

    helper::drawTexture(m_buttons.at(static_cast<std::size_t>(m_selected)));

    drawGameStats();

    ImGui::End();


    if (close()) {
        m_selected = Button::MENU;
        forceSelect(true);
    }
    if (select()) {
        holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/menu/accept.wav")->play();
        clean_world(world);

        switch (m_selected) {
        case Button::PLAY_AGAIN:
            game.setMenu(nullptr);
            game.logics()->onGameStart.publish(world);
            return;
        case Button::MENU: game.setMenu(std::make_unique<menu::MainMenu>()); return;
        }
    }
}

void game::menu::GameOver::drawGameStats()
{
    const auto time = m_stats.gameTimeInSeconds;

    const int min = static_cast<int>(time / 60);
    const int sec = static_cast<int>(time - min * 60);
    const int ms = static_cast<int>((time - min * 60 - sec) * 1000);

    helper::drawText(
        helper::frac2pixel(helper::from1080p(1018, 321)),
        fmt::format("{}m {}.{}s", min, sec, ms),
        ImVec4(1, 1, 1, 1),
        Fonts::kimberley_62);

    helper::drawText(
        helper::frac2pixel(helper::from1080p(1018, 392)),
        fmt::format("{}", m_stats.finalLevel),
        ImVec4(1, 1, 1, 1),
        Fonts::kimberley_62);

    helper::drawText(
        helper::frac2pixel(helper::from1080p(1018, 463)),
        fmt::format("{}", m_stats.enemyKilled),
        ImVec4(1, 1, 1, 1),
        Fonts::kimberley_62);
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
            [&](const engine::TimeElapsed &dt) { m_timeElapsed += static_cast<double>(dt.elapsed.count()) / 1e9; },
            [&](auto) {},
        },
        e);
}
