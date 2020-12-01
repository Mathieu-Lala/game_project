#pragma once

#include <entt/entt.hpp>
#include "component/Health.hpp"
#include "component/Level.hpp"

#include "ThePURGE.hpp"

namespace game {

struct GameHUD {
    static void draw(ThePURGE &game, entt::registry &world);
    
private:
    static void drawHealthBar(const Health &health);
    static void drawXpBar(const Level &level);
};

} // namespace game
