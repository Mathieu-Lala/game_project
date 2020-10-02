#include "level/LevelTilemapBuilder.hpp"
#include <cassert>

#include "level/TileFactory.hpp"

auto TilemapBuilder::get(int x, int y) -> TileEnum &
{
    assert(x < m_maxWidth);
    assert(y < m_maxHeight);

    return m_tiles[y * m_maxWidth + x];
}

void TilemapBuilder::build(entt::registry &world)
{
    for (int y = 0; y < m_maxHeight; ++y)
        for (int x = 0; x < m_maxWidth; ++x) handleTileBuild(world, x, y);
}

void TilemapBuilder::handleTileBuild(entt::registry &world, int x, int y)
{
    auto tile = get(x, y);
    if (tile == TileEnum::NONE) return;

    auto size = getTileSize(x, y);

    for (int clearY = y; clearY < y + size.y; ++clearY)
        for (int clearX = x; clearX < x + size.x; ++clearX) get(clearX, clearY) = TileEnum::NONE;


    switch (tile) {
    case TileEnum::FLOOR: TileFactory::Floor(world, {x, y}, size); break;
    case TileEnum::WALL: TileFactory::Wall(world, {x, y}, size); break;

    default: break;
    }
}

glm::vec2 TilemapBuilder::getTileSize(int x1, int y1)
{
    auto tile = get(x1, y1);

    int x2 = x1 + 1;
    int y2 = y1 + 1;

    while (x2 < m_maxWidth && get(x2, y1) == tile) ++x2;

    while (y2 < m_maxWidth) {
        bool isEntireLineSame = true;

        for (int x = x1; x < x2; ++x) {
            if (get(x, y2) != tile) {
                isEntireLineSame = false;
                break;
            }
        }

        if (!isEntireLineSame) break;
        ++y2;
    }

    return {x2 - x1, y2 - y1};
}