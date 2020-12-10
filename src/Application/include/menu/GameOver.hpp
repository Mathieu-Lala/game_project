#pragma once

#include <entt/entt.hpp>

#include "AMenu.hpp"
#include "widgets/helpers.hpp"
#include "models/EndGameStats.hpp"

namespace game::menu {

class GameOver : public AMenu {
public:
    GameOver(const EndGameStats &stats) : m_stats(stats) {}

    void create(entt::registry &world, ThePURGE &game) final;
    void draw(entt::registry &world, ThePURGE &game) final;
    void event(entt::registry &world, ThePURGE &game, const engine::Event &e) final;

private:
    void drawGameStats();

    auto clean_world(entt::registry &) -> void;

private:
    enum Button {
        PLAY_AGAIN = 0,
        MENU,

        MAX // last value, not a real button
    };

    std::uint32_t m_backgroundTexture;
    std::vector<GUITexture> m_buttons;

    int m_selected = Button::PLAY_AGAIN;

    EndGameStats m_stats;

    double m_timeElapsed = 0;
};

} // namespace game::menu
