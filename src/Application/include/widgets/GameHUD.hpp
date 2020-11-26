#pragma once

#include <entt/entt.hpp>

#include "ThePURGE.hpp"

namespace game {

struct GameHUD {
    static void draw(ThePURGE &game, entt::registry &world);
    
};

} // namespace game
