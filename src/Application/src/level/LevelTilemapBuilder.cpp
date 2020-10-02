#include "level/LevelTilemapBuilder.hpp"
#include <cassert>

#include "level/TileFactory.hpp"

auto TilemapBuilder::get(int x, int y) -> TileEnum &
{
    assert(x < kMaxWidth);
    assert(y < kMaxHeight);

    return m_tiles[y * kMaxWidth + x];
}

void TilemapBuilder::build(entt::registry &world)
{
    for (int y = 0; y < kMaxHeight; ++y)
        for (int x = 0; x < kMaxWidth; ++x) {
            switch (get(x, y)) {
            case TileEnum::FLOOR: TileFactory::Floor(world, {x, y}, {1, 1}); break;
            case TileEnum::WALL: TileFactory::Wall(world, {x, y}, {1, 1}); break;

            default: break;
            }
        }
}
