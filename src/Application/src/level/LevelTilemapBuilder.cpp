#include "level/LevelTilemapBuilder.hpp"
#include <cassert>

#include "level/TileFactory.hpp"

auto TilemapBuilder::get(int x, int y) -> TileEnum &
{
    assert(x < m_size.x);
    assert(y < m_size.y);

    return m_tiles[static_cast<std::size_t>(y * m_size.x + x)];
}

void TilemapBuilder::build(entt::registry &world)
{
    for (int y = 0; y < m_size.y; ++y) {
        for (int x = 0; x < m_size.x; ++x) { handleTileBuild(world, x, y); }
    }
}

void TilemapBuilder::handleTileBuild(entt::registry &world, int x, int y)
{
    auto tile = get(x, y);
    if (tile == TileEnum::NONE) return;

    auto size = getTileSize(x, y);

    for (int clearY = y; clearY < y + size.y; ++clearY) {
        for (int clearX = x; clearX < x + size.x; ++clearX) { get(clearX, clearY) = TileEnum::NONE; }
    }

    switch (tile) {
    case TileEnum::FLOOR:
        TileFactory::Floor(
            world,
            m_shader,
            {static_cast<float>(x) / 100.0f, static_cast<float>(y) / 100.0f},
            {static_cast<float>(size.x) / 10.0f, static_cast<float>(size.y) / 10.0f});
        break;
    case TileEnum::WALL:
        TileFactory::Wall(
            world,
            m_shader,
            {static_cast<float>(x) / 100.0f, static_cast<float>(y) / 100.0f},
            {static_cast<float>(size.x) / 10.0f, static_cast<float>(size.y) / 10.0f});
        break;

    default: break;
    }
}

glm::ivec2 TilemapBuilder::getTileSize(int x1, int y1)
{
    auto tile = get(x1, y1);

    int x2 = x1 + 1;
    int y2 = y1 + 1;

    while (x2 < m_size.x && get(x2, y1) == tile) ++x2;

    while (y2 < m_size.x) {
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
