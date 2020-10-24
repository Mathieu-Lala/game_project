#include "level/MapGenerator.hpp"
#include <vector>
#include <algorithm>
#include <random>
#include <cassert>
#include "level/LevelTilemapBuilder.hpp"
#include "entity/EnemyFactory.hpp"
#include "level/MapData.hpp"

// todo : move this in Core ?
static std::default_random_engine random_engine{};

// Generate random int in the interval [min; max[
template<std::integral T>
auto randRange(T min, T max)
{
    assert(max > min);

    const auto r = random_engine();
    return min + static_cast<T>(r % static_cast<decltype(r)>(max - min));
}

bool isRoomValid(game::TilemapBuilder &builder, const game::Room &r)
{
    int x2 = r.x + r.w;
    int y2 = r.y + r.h;

    for (int x = r.x; x < x2; ++x)
        for (int y = r.y; y < y2; ++y)
            if (builder.get(x, y) != game::TileEnum::NONE) return false;

    return true;
}

game::Room generateRoom(game::TilemapBuilder &builder, const game::FloorGenParam &params)
{
    game::Room r;

    std::uint32_t tries = 0;
    do {
        if (tries++ > 10000) return {0, 0, 0, 0};

        // Width including walls (hence the +2)
        r.w = randRange(params.minRoomSize + 2, params.maxRoomSize + 2 + 1);
        r.h = randRange(params.minRoomSize + 2, params.maxRoomSize + 2 + 1);

        r.x = randRange(0, params.maxDungeonWidth - r.w);
        r.y = randRange(0, params.maxDungeonHeight - r.h);
    } while (!isRoomValid(builder, r));

    return r;
}

void placeRoomFloor(game::TilemapBuilder &builder, const game::Room &r, game::TileEnum floorTile)
{
    int x2 = r.x + r.w;
    int y2 = r.y + r.h;

    for (int y = r.y + 1; y < y2 - 1; ++y)
        for (int x = r.x + 1; x < x2 - 1; ++x) builder.get(x, y) = floorTile;
}

// If gap is even, center will be chosen randomly between the two center tiles
int getOnePossibleCenterOf(int a, int b)
{
    int result = (a + b) / 2;

    if ((a - b) % 2 == 0) result -= randRange(0, 1);

    return result;
}

// If room size is even, center will be chosen randomly between the two center tiles
glm::ivec2 getOnePossibleCenterOf(const game::Room &r)
{
    glm::ivec2 pos;

    pos.x = getOnePossibleCenterOf(r.x, r.x + r.w);
    pos.y = getOnePossibleCenterOf(r.y, r.y + r.h);

    return pos;
}

void generatorCorridor(
    game::TilemapBuilder &builder, const game::FloorGenParam &params, const game::Room &r1, const game::Room &r2)
{
    auto start = getOnePossibleCenterOf(r1);
    auto end = getOnePossibleCenterOf(r2);

    auto vertical = [&](glm::vec<2, int> pos, int width) -> glm::vec<2, int> {
        auto widthOffset = getOnePossibleCenterOf(0, width);

        auto minX = pos.x - widthOffset;
        auto maxX = pos.x + (width - widthOffset);

        while (pos.y != end.y) {
            for (int x = minX; x < maxX; ++x)
                if (builder.get(x, pos.y) == game::TileEnum::NONE)
                    builder.get(x, pos.y) = game::TileEnum::FLOOR_CORRIDOR;

            if (pos.y < end.y)
                ++pos.y;
            else
                --pos.y;
        }

        return pos;
    };

    auto horizontal = [&](glm::vec<2, int> pos, int width) -> glm::vec<2, int> {
        auto widthOffset = getOnePossibleCenterOf(0, width);

        auto minY = pos.y - widthOffset;
        auto maxY = pos.y + (width - widthOffset);

        while (pos.x != end.x) {
            for (int y = minY; y < maxY; ++y)
                if (builder.get(pos.x, y) == game::TileEnum::NONE)
                    builder.get(pos.x, y) = game::TileEnum::FLOOR_CORRIDOR;

            if (pos.x < end.x)
                ++pos.x;
            else
                --pos.x;
        }

        return pos;
    };


    if (randRange(0, 1)) {
        auto maxWidth = std::min(
            r1.w - 3, r2.h - 3); // -2 for walls, and -1 to be safe about the random center not making wall go off bound
        auto width =
            randRange(std::min(maxWidth, params.minCorridorWidth), std::min(maxWidth + 1, params.maxCorridorWidth + 1));

        auto pos = vertical(start, width);
        horizontal(pos, width);
    } else {
        auto maxWidth = std::min(
            r1.h - 3, r2.w - 3); // -2 for walls, and -1 to be safe about the random center not making wall go off bound
        auto width =
            randRange(std::min(maxWidth, params.minCorridorWidth), std::min(maxWidth + 1, params.maxCorridorWidth + 1));

        auto pos = horizontal(start, width);
        vertical(pos, width);
    }
}

void placeWalls(game::TilemapBuilder &builder)
{
    // For each empty tile, if one it's neighbour is a floor, make it a wall
    std::array<glm::vec<2, int>, 8> neighbours = {
        glm::vec<2, int>{-1, -1},
        glm::vec<2, int>{0, -1},
        glm::vec<2, int>{1, -1},
        glm::vec<2, int>{-1, 0},
        glm::vec<2, int>{1, 0},
        glm::vec<2, int>{-1, 1},
        glm::vec<2, int>{0, 1},
        glm::vec<2, int>{1, 1}};

    glm::vec<2, int> it;

    for (it.y = 0; it.y < builder.getSize().y; ++it.y)
        for (it.x = 0; it.x < builder.getSize().x; ++it.x) {
            if (builder.get(it.x, it.y) != game::TileEnum::NONE && builder.get(it.x, it.y) != game::TileEnum::RESERVED)
                continue;

            for (auto n : neighbours) {
                auto checkPos = it + n;

                if (checkPos.x > 0 && checkPos.x < builder.getSize().x && checkPos.y > 0
                    && checkPos.y < builder.getSize().y && IS_FLOOR(builder.get(checkPos.x, checkPos.y))) {
                    builder.get(it.x, it.y) = game::TileEnum::WALL;
                    break;
                }
            }
        }
}

void placeBossRoomExitDoor(game::TilemapBuilder &builder, game::Room &r)
{
    // Strategy :
    //  Check room boundaries, find the boss room entrance, place a door at the central opposite of the room with the right orientation
    //  Reduce room size by one to make sure the door is not accessible by another room on the other side of the wall

    int x1 = r.x;
    int x2 = r.x + r.w;
    int y1 = r.y;
    int y2 = r.y + r.h;

    // CHECK NORTH WALL
    for (auto x = x1 + 1; x < x2 - 1; ++x)
        if (IS_FLOOR(builder.get(x, y2 - 1))) {
            r.y += 1;
            r.h -= 1;

            builder.get(x2 - 1 - (x - x1), r.y) = game::TileEnum::EXIT_DOOR_FACING_NORTH;
            return;
        }

    // CHECK SOURTH WALL
    for (auto x = x1 + 1; x < x2 - 1; ++x)
        if (IS_FLOOR(builder.get(x, y1))) {
            r.h -= 1;

            builder.get(x2 - 1 - (x - x1), r.y + r.h - 1) = game::TileEnum::EXIT_DOOR_FACING_SOUTH;
            return;
        }

    // CHECK WEST WALL
    for (auto y = y1 + 1; y < y2 - 1; ++y)
        if (IS_FLOOR(builder.get(x1, y))) {
            r.w -= 1;

            builder.get(r.x + r.w - 1, y2 - 1 - (y - y1)) = game::TileEnum::EXIT_DOOR_FACING_WEST;
            return;
        }

    // CHECK EAST WALL
    for (auto y = y1 + 1; y < y2 - 1; ++y)
        if (IS_FLOOR(builder.get(x2 - 1, y))) {
            r.x += 1;
            r.w -= 1;

            builder.get(r.x, y2 - 1 - (y - y1)) = game::TileEnum::EXIT_DOOR_FACING_EAST;
            return;
        }

    assert(false && "Could not find boss room entrance, cannot place exit door");
}

game::MapData generateLevel(entt::registry &world, game::FloorGenParam params)
{
    game::TilemapBuilder builder({params.maxDungeonWidth, params.maxDungeonHeight});

    auto roomCount = randRange(params.minRoomCount, params.maxRoomCount);

    std::vector<game::Room> rooms;
    rooms.reserve(roomCount);

    rooms.emplace_back(generateRoom(builder, params));
    placeRoomFloor(builder, rooms[0], game::TileEnum::RESERVED);

    for (std::size_t i = 0; i < roomCount - 1; ++i) {
        auto room = generateRoom(builder, params);
        if (room.w == 0 && room.h == 0) break; // Failed to place room

        placeRoomFloor(
            builder, room, game::TileEnum::RESERVED); // Reserve the space, so multiple rooms don't spawn at the same place
        rooms.emplace_back(room);

        generatorCorridor(builder, params, rooms[i], rooms[i + 1]);
    }

    game::MapData result;
    result.spawn = *rooms.begin();
    result.boss = *rooms.rbegin();

    for (auto i = 1ul; i < rooms.size() - 1; ++i) result.regularRooms.push_back(rooms[i]);

    placeRoomFloor(builder, result.spawn, game::TileEnum::FLOOR_SPAWN);
    for (const auto &r : result.regularRooms) placeRoomFloor(builder, r, game::TileEnum::FLOOR_NORMAL_ROOM);

    placeBossRoomExitDoor(builder, result.boss);
    placeRoomFloor(builder, result.boss, game::TileEnum::FLOOR_BOSS_ROOM);

    placeWalls(builder);

    builder.build(world);

    return result;
}

void spawnMobsIn(entt::registry &world, game::FloorGenParam params, const game::Room &r)
{
    if (params.mobDensity == 0)
        return;

    for (auto x = r.x + 1; x < r.x + r.w - 1; ++x)
        for (auto y = r.y + 1; y < r.y + r.h - 1; ++y)
            if (randRange(0, static_cast<int>(1.0f / params.mobDensity)) == 0)
                game::EnemyFactory::FirstEnemy(world, glm::vec2{x + 0.5, y + 0.5});
}

auto game::generateFloor(entt::registry &world, game::FloorGenParam params, std::optional<unsigned int> seed) -> MapData
{
    if (seed) random_engine.seed(seed.value());

    auto data = generateLevel(world, params);

    for (auto &r : data.regularRooms) spawnMobsIn(world, params, r);
    game::EnemyFactory::Boss(world, glm::vec2{data.boss.x + data.boss.w * 0.5, data.boss.y + data.boss.h * 0.5});

    data.nextFloorSeed = static_cast<unsigned int>(random_engine());

    return data;
}
