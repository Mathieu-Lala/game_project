#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

namespace engine {
class Shader;
}

class TileFactory {
public:
    void static Floor(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size);
    void static Wall(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size);

    static auto getShader() -> engine::Shader *;

};
