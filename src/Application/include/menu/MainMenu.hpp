#pragma once

#include <entt/entt.hpp>

#include "AMenu.hpp"

namespace game::menu {

class MainMenu : public AMenu {
public:
    bool draw(entt::registry &world, ThePURGE &game) final;
    bool event(entt::registry &world, ThePURGE &game, const engine::Event &e) final;

private:

};

} // namespace game
