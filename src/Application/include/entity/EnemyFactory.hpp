#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

#include "Engine/Graphics/Shader.hpp"

namespace game {

class EnemyFactory { // note : should be merged with TileFactory.hpp
public:
    static void Boss(entt::registry &world, const glm::vec2 &pos);
    static void FirstEnemy(entt::registry &world, const glm::vec2 &pos);
};

} // namespace game
