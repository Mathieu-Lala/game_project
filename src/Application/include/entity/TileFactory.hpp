#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

#include "Engine/Graphics/Shader.hpp"

namespace game {

class TileFactory {
public:
    static void Floor(entt::registry &world, engine::Shader *, const glm::vec2 &pos, const glm::vec2 &size);
    static void Wall(entt::registry &world, engine::Shader *, const glm::vec2 &pos, const glm::vec2 &size);
};

} // namespace game
