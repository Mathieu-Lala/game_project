#pragma once

#include <optional>
#include <entt/entt.hpp>

#include "Engine/Shader.hpp"

struct FloorGenParam {
    int minRoomSize = 7; // exluding walls
    int maxRoomSize = 15; // exluding walls

    std::size_t minRoomCount = 5; // including boss room
    std::size_t maxRoomCount = 10; // including boss room

    int maxDungeonWidth = 50;
    int maxDungeonHeight = 50;

    int minCorridorWidth = 2;
    int maxCorridorWidth = 4;
};

void generateFloor(entt::registry &world, engine::Shader *, FloorGenParam params = {}, std::optional<unsigned int> seed = {});
