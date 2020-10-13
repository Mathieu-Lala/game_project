#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

#include "Engine/Shader.hpp"

class EnemyFactory {
public:
    static void FirstEnemy(entt::registry &world, engine::Shader *, const glm::vec2 &pos);
};
