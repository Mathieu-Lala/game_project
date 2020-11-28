#pragma once

#include <entt/entt.hpp>

#include "AMenu.hpp"

namespace game::menu {

class Credits : public AMenu {
public:
    void create(entt::registry &world, ThePURGE &game) final;
    void draw(entt::registry &world, ThePURGE &game) final;
    void event(entt::registry &world, ThePURGE &game, const engine::Event &e) final;

private:
    std::uint32_t m_texture;

    bool m_exit = false;
};

} // namespace game
