#include <cassert>

#include "level/LevelTilemapBuilder.hpp"
#include "entity/TileFactory.hpp"

auto game::TilemapBuilder::get(int x, int y) -> TileEnum &
{
    assert(x < m_size.x);
    assert(y < m_size.y);

    return m_tiles[static_cast<std::size_t>(y) * static_cast<std::size_t>(m_size.x) + static_cast<std::size_t>(x)];
}

void game::TilemapBuilder::build(entt::registry &world)
{
    for (auto y = 0; y < m_size.y; ++y) {
        for (auto x = 0; x < m_size.x; ++x) { handleTileBuild(world, x, y); }
    }
}

void game::TilemapBuilder::handleTileBuild(entt::registry &world, int x, int y)
{
    auto tile = get(x, y);
    if (tile == TileEnum::NONE) return;

    glm::ivec2 size(1, 1);

    // TODO: investigate why this doesn't work. It used to work before, problem probably comes from camera (?)
    // auto size = getTileSize(x, y);

    // for (int clearY = y; clearY < y + size.y; ++clearY) {
    //    for (int clearX = x; clearX < x + size.x; ++clearX) { get(clearX, clearY) = TileEnum::NONE; }
    //}

    glm::vec2 tilePos{static_cast<float>(x), static_cast<float>(y)};
    glm::vec2 tileSize{static_cast<float>(size.x), static_cast<float>(size.y)};

    switch (tile) {
    case TileEnum::FLOOR_NORMAL_ROOM: TileFactory::FloorNormalRoom(world, m_shader, tilePos, tileSize); break;
    case TileEnum::FLOOR_BOSS_ROOM: TileFactory::FloorBossRoom(world, m_shader, tilePos, tileSize); break;
    case TileEnum::FLOOR_CORRIDOR: TileFactory::FloorCorridor(world, m_shader, tilePos, tileSize); break;
    case TileEnum::FLOOR_SPAWN: TileFactory::FloorSpawnRoom(world, m_shader, tilePos, tileSize); break;

    case TileEnum::EXIT_DOOR_FACING_NORTH: TileFactory::ExitDoor(world, m_shader, tilePos, tileSize, 0); break;
    case TileEnum::EXIT_DOOR_FACING_EAST: TileFactory::ExitDoor(world, m_shader, tilePos, tileSize, 90); break;
    case TileEnum::EXIT_DOOR_FACING_SOUTH: TileFactory::ExitDoor(world, m_shader, tilePos, tileSize, 180); break;
    case TileEnum::EXIT_DOOR_FACING_WEST: TileFactory::ExitDoor(world, m_shader, tilePos, tileSize, 270); break;

    case TileEnum::DEBUG_TILE: TileFactory::DebugTile(world, m_shader, tilePos, tileSize); break;
    case TileEnum::WALL: TileFactory::Wall(world, m_shader, tilePos, tileSize); break;

    default: break;
    }
}

glm::ivec2 game::TilemapBuilder::getTileSize(int x1, int y1)
{
    auto tile = get(x1, y1);

    int x2 = x1 + 1;
    int y2 = y1 + 1;

    while (x2 < m_size.x && get(x2, y1) == tile) ++x2;

    while (y2 < m_size.x) {
        bool isEntireLineSame = true;

        for (auto x = x1; x < x2; ++x) {
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
