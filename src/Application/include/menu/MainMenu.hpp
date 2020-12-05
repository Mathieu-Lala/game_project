#pragma once

#include <entt/entt.hpp>

#include "AMenu.hpp"
#include "widgets/helpers.hpp"

namespace game::menu {

class MainMenu : public AMenu {
public:
    void create(entt::registry &world, ThePURGE &game) final;
    void draw(entt::registry &world, ThePURGE &game) final;
    void event(entt::registry &world, ThePURGE &game, const engine::Event &e) final;

private:
    enum Button {
        PLAY = 0,
        HOWTOPLAY,
        CREDITS,
        EXIT,

        MAX // last value, not a real button
    };


    std::uint32_t m_backgroundTexture;
    std::vector<GUITexture> m_buttons;

    int m_selected = Button::PLAY;
};

} // namespace game
