#pragma once

#include <entt/entt.hpp>

#include "AMenu.hpp"

namespace game::menu {

class GameOver : public AMenu {
public:
    void draw(entt::registry &world, ThePURGE &game) override;
    void event(entt::registry &world, ThePURGE &game, const engine::Event &e) override;

};

} // namespace game
