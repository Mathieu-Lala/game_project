#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

#include "Engine/Shader.hpp"

namespace engine {
class Shader;
}

class TileFactory {
public:
    static void Floor(entt::registry &world, engine::Shader *, glm::vec2 &&pos, glm::vec2 &&size);
    static void Wall(entt::registry &world, engine::Shader *, glm::vec2 &&pos, glm::vec2 &&size);

};
