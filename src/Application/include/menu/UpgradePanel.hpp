#pragma once

#include <entt/entt.hpp>

#include "AMenu.hpp"
#include "widgets/helpers.hpp"

namespace game::menu {

class UpgradePanel : public AMenu {
public:
    void create(entt::registry &world, ThePURGE &game) final;
    void draw(entt::registry &world, ThePURGE &game) final;
    void event(entt::registry &world, ThePURGE &game, const engine::Event &e) final;

private:
    GUITexture m_static_background;
};

} // namespace game
