#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

#include "Engine/Graphics/Shader.hpp"

namespace game {

class EnemyFactory {
public:
    static void FirstEnemy(entt::registry &world, engine::Shader *, const glm::vec2 &pos);
    static void Boss(entt::registry &world, engine::Shader *, const glm::vec2 &pos);
};

} // namespace game
