#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

#include "Engine/Graphics/Shader.hpp"

namespace game {

class EnemyFactory {
public:
<<<<<<< HEAD
    static void FirstEnemy(entt::registry &world, engine::Shader *, const glm::vec2 &pos);
    static void Boss(entt::registry &world, engine::Shader *, const glm::vec2 &pos);
=======
    static void FirstEnemy(entt::registry &world, const glm::vec2 &pos);
>>>>>>> 2ef832090ceee2b5f1494879689d876d5e658aae
};

} // namespace game
