#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

namespace game {

class TileFactory {
public:
    // note : this could be simpler..

    static void FloorNormalRoom(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size);
    static void FloorSpawnRoom(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size);
    static void FloorBossRoom(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size);
    static void FloorCorridor(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size);

    static void ExitDoor(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size, float rotation);
    static void Wall(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size);

    static void DebugTile(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size);
};

} // namespace game
