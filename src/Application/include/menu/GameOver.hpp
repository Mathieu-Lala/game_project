#pragma once

#include <entt/entt.hpp>

#include "AMenu.hpp"

namespace game::menu {

class GameOver : public AMenu {
public:
    void create(entt::registry &world, ThePURGE &game) final;
    ~GameOver();

    void draw(entt::registry &world, ThePURGE &game) final;
    void event(entt::registry &world, ThePURGE &game, const engine::Event &e) final;


private:

    auto clean_world(entt::registry &) -> void;

};

} // namespace game
