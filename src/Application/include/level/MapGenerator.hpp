#pragma once

#include <optional>
#include <entt/entt.hpp>

struct FloorGenParam {
    int minRoomSize = 7; // exluding walls
    int maxRoomSize = 15; // exluding walls

    int minRoomCount = 5; // including boss room 
    int maxRoomCount = 10; // including boss room

    int maxDungeonWidth = 50;
    int maxDungeonheight = 50;
};

void generateFloor(entt::registry &world, FloorGenParam params = {}, std::optional<unsigned int> seed = {});
