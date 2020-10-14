﻿#pragma once

#include <vector>

namespace game {

/**
    In the range [x;x+w[ and [y;y+h[
    include walls
*/
struct Room {
    int x, y;
    int w, h;
};


struct MapData {
    Room spawn{};
    std::vector<Room> regularRooms;
    Room boss{};
};

} // namespace game
