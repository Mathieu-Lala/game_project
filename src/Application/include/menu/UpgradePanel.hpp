#pragma once

#include <entt/entt.hpp>

#include "AMenu.hpp"

namespace game::menu {

class UpgradePanel : public AMenu {
public:
    void draw(entt::registry &world, ThePURGE &game) final;
    void event(entt::registry &world, ThePURGE &game, const engine::Event &e) final;

private:

};

} // namespace game
