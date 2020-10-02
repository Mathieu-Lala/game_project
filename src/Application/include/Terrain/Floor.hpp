#pragma once

#include "Tile.hpp"

class Floor : public Tile {

public:
    Floor(entt::registry &world, const glm::vec2 &pos);


private:
    static constexpr glm::vec3 kColor{1, 1, 1};
};