#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

#include "Engine/Graphics/Shader.hpp"

namespace game {

class TileFactory {
public:
    static void FloorNormalRoom(entt::registry &world, engine::Shader *, const glm::vec2 &pos, const glm::vec2 &size);
    static void FloorSpawnRoom(entt::registry &world, engine::Shader *, const glm::vec2 &pos, const glm::vec2 &size);
    static void FloorBossRoom(entt::registry &world, engine::Shader *, const glm::vec2 &pos, const glm::vec2 &size);
    static void FloorCorridor(entt::registry &world, engine::Shader *, const glm::vec2 &pos, const glm::vec2 &size);

    static void Wall(entt::registry &world, engine::Shader *, const glm::vec2 &pos, const glm::vec2 &size);
};

} // namespace game
