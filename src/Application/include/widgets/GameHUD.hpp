#pragma once

#include <entt/entt.hpp>
#include "component/Health.hpp"

#include "ThePURGE.hpp"

namespace game {

struct GameHUD {
    static void draw(ThePURGE &game, entt::registry &world);
    
private:
    static void drawHealthBar(const Health &health);
};

} // namespace game
