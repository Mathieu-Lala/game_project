#pragma once

#include <optional>
#include <entt/entt.hpp>

#include "Engine/Graphics/Shader.hpp"
#include "level/MapData.hpp"

namespace game {

struct FloorGenParam {
    int minRoomSize = 7;  // exluding walls
    int maxRoomSize = 15; // exluding walls

    std::size_t minRoomCount = 5;  // including boss room
    std::size_t maxRoomCount = 10; // including boss room

    int maxDungeonWidth = 50;
    int maxDungeonHeight = 50;

    int minCorridorWidth = 2;
    int maxCorridorWidth = 4;


    float mobDensity = 0.05f; // Average mob per tile
};

auto generateFloor(entt::registry &world, FloorGenParam params = {}, std::optional<unsigned int> seed = {})
    -> MapData;

} // namespace game
