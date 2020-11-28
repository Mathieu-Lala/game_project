#pragma once

#include <entt/entt.hpp>

#include "AMenu.hpp"

namespace game::menu {

class HowToPlay : public AMenu {
public:
    void create(entt::registry &world, ThePURGE &game) final;
    void draw(entt::registry &world, ThePURGE &game) final;
    void event(entt::registry &world, ThePURGE &game, const engine::Event &e) final;

private:
    enum class Tab {
        HOW_TO_PLAY,
        CONTROLS
    };

    std::uint32_t m_howToPlay;
    std::uint32_t m_controls;

    Tab m_currentTab = Tab::HOW_TO_PLAY;
};

} // namespace game
