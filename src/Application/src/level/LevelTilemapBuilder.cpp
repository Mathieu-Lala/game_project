#include "level/LevelTilemapBuilder.hpp"
#include "factory/EntityFactory.hpp"

void game::TilemapBuilder::build(entt::registry &world)
{
    for (auto y = 0; y < m_size.y; ++y) {
        for (auto x = 0; x < m_size.x; ++x) { handleTileBuild(world, x, y); }
    }
}

void game::TilemapBuilder::handleTileBuild(entt::registry &world, int x, int y)
{
    const auto tile = at(glm::ivec2{x, y});
    if (tile == TileEnum::NONE) return;

    auto size = getTileSize(x, y);

    for (auto clearY = y; clearY < y + size.y; ++clearY) {
        for (auto clearX = x; clearX < x + size.x; ++clearX) {
            operator[](glm::ivec2{clearX, clearY}) = TileEnum::NONE;
        }
    }

    glm::vec2 tileSize{static_cast<float>(size.x), static_cast<float>(size.y)};
    glm::vec2 tilePos{static_cast<float>(x), static_cast<float>(y)};

    tilePos += tileSize / 2.f;

    switch (tile) {
    case TileEnum::FLOOR_NORMAL_ROOM:
        EntityFactory::create<EntityFactory::FLOOR_NORMAL>(world, tilePos, tileSize);
        break;
    case TileEnum::FLOOR_BOSS_ROOM: EntityFactory::create<EntityFactory::FLOOR_BOSS>(world, tilePos, tileSize); break;
    case TileEnum::FLOOR_CORRIDOR:
        EntityFactory::create<EntityFactory::FLOOR_CORRIDOR>(world, tilePos, tileSize);
        break;
    case TileEnum::FLOOR_SPAWN: EntityFactory::create<EntityFactory::FLOOR_SPAWN>(world, tilePos, tileSize); break;

    case TileEnum::EXIT_DOOR_FACING_NORTH:
        EntityFactory::create<EntityFactory::EXIT_DOOR>(world, tilePos, tileSize /*, 0*/);
        break;
    case TileEnum::EXIT_DOOR_FACING_EAST:
        EntityFactory::create<EntityFactory::EXIT_DOOR>(world, tilePos, tileSize /*, 90*/);
        break;
    case TileEnum::EXIT_DOOR_FACING_SOUTH:
        EntityFactory::create<EntityFactory::EXIT_DOOR>(world, tilePos, tileSize /*, 180*/);
        break;
    case TileEnum::EXIT_DOOR_FACING_WEST:
        EntityFactory::create<EntityFactory::EXIT_DOOR>(world, tilePos, tileSize /*, 270*/);
        break;

    case TileEnum::DEBUG_TILE: EntityFactory::create<EntityFactory::DEBUG_TILE>(world, tilePos, tileSize); break;
    case TileEnum::WALL: EntityFactory::create<EntityFactory::WALL>(world, tilePos, tileSize); break;

    default: break;
    }
}

auto game::TilemapBuilder::getTileSize(int x, int y) const -> glm::ivec2
{
    const auto tile = at(glm::ivec2{x, y});

    auto x2 = x + 1;
    auto y2 = y + 1;

    while (x2 < m_size.x && at(glm::ivec2{x2, y}) == tile) ++x2;

    while (y2 < m_size.x) {
        bool isEntireLineSame = true;

        for (auto i = x; i < x2; ++i) {
            if (at(glm::ivec2{i, y2}) != tile) {
                isEntireLineSame = false;
                break;
            }
        }

        if (!isEntireLineSame) break;
        ++y2;
    }

    return {x2 - x, y2 - y};
}
