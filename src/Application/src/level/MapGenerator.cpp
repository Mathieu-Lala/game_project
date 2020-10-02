#include "level/MapGenerator.hpp"
#include <vector>
#include <random>
#include "level/LevelTilemapBuilder.hpp"

/**
    In the range [x;x+w[ and [y;y+h[
    include walls
*/
struct Room {
    int x, y;
    int w, h;
};

// Generate random int in the interval [min; max[
auto randRange(int min, int max, std::default_random_engine &randomEngine)
{
    return min + randomEngine() % (max - min);
}

bool isRoomValid(TilemapBuilder &builder, const Room &r)
{
    int x2 = r.x + r.w;
    int y2 = r.y + r.h;

    for (int x = r.x; x <= x2; ++x)
        if (builder.get(x, y2) != TileEnum::NONE) return false;
    for (int x = r.x; x <= x2; ++x)
        if (builder.get(x, r.y) != TileEnum::NONE) return false;

    for (int y = r.y + 1; y < y2; ++y)
        if (builder.get(r.x, y) != TileEnum::NONE) return false;
    for (int y = r.y + 1; y < y2; ++y)
        if (builder.get(x2, y) != TileEnum::NONE) return false;

    return true;
}

Room generateRoom(TilemapBuilder &builder, const FloorGenParam &params, std::default_random_engine &randomEngine)
{
    Room r;

    unsigned int tries = 0;
    do {
        if (tries++ > 10000) return {0, 0, 0, 0};

        // Width including walls (hence the +2)
        r.w = randRange(params.minRoomSize + 2, params.maxRoomSize + 2, randomEngine);
        r.h = randRange(params.minRoomSize + 2, params.maxRoomSize + 2, randomEngine);

        r.x = randRange(0, builder.getMaxWidth() - r.w, randomEngine);
        r.y = randRange(0, builder.getMaxHeight() - r.h, randomEngine);


    } while (!isRoomValid(builder, r));

    int x2 = r.x + r.w;
    int y2 = r.y + r.h;

    for (int y = r.y + 1; y < y2 - 1; ++y)
        for (int x = r.x + 1; x < x2 - 1; ++x) builder.get(x, y) = TileEnum::FLOOR;

    return r;
}

// If room size is even, center will be chosen randomly between the two center tiles
glm::vec<2, int> getOnePossibleCenterOf(const Room &r, std::default_random_engine &randomEngine)
{
    glm::vec<2, int> pos = glm::vec2(r.x, r.y) + (glm::vec2(r.w, r.h) / 2.f);

    if (r.w % 2) pos.x -= randRange(0, 1, randomEngine);
    if (r.h % 2) pos.x -= randRange(0, 1, randomEngine);

    return pos;
}

void generatorCorridor(
    TilemapBuilder &builder, const FloorGenParam &params, std::default_random_engine &randomEngine, const Room &r1, const Room &r2)
{
    auto start = getOnePossibleCenterOf(r1, randomEngine);
    auto end = getOnePossibleCenterOf(r2, randomEngine);

    auto vertical = [&](glm::vec2 pos) -> glm::vec2 {
        while (pos.y != end.y) {
            builder.get(pos.x, pos.y) = TileEnum::FLOOR;

            if (pos.y < end.y)
                ++pos.y;
            else
                --pos.y;
        }

        return pos;
    };

    auto horizontal = [&](glm::vec2 pos) -> glm::vec2 {
        while (pos.x != end.x) {
            builder.get(pos.x, pos.y) = TileEnum::FLOOR;

            if (pos.x < end.x)
                ++pos.x;
            else
                --pos.x;
        }

        return pos;
    };


    if (randRange(0, 1, randomEngine)) {
        auto pos = vertical(start);
        horizontal(pos);
    } else {
        auto pos = horizontal(start);
        vertical(pos);
    }
}

void placeWalls(TilemapBuilder &builder)
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

    for (it.y = 0; it.y < builder.getMaxHeight(); ++it.y)
        for (it.x = 0; it.x < builder.getMaxWidth(); ++it.x) {
            if (builder.get(it.x, it.y) != TileEnum::NONE) continue;

            for (auto n : neighbours) {
                auto checkPos = it + n;

                if (checkPos.x > 0 && checkPos.x < builder.getMaxWidth() && checkPos.y > 0
                    && checkPos.y < builder.getMaxHeight() && builder.get(checkPos.x, checkPos.y) == TileEnum::FLOOR) {
                    builder.get(it.x, it.y) = TileEnum::WALL;
                    break;
                }
            }
        }
}

void generateFloor(entt::registry &world, FloorGenParam params, std::optional<unsigned int> seed)
{
    TilemapBuilder builder(params.maxDungeonWidth, params.maxDungeonheight);
    std::default_random_engine randomEngine;

    if (seed) randomEngine.seed(seed.value());

    std::vector<Room> rooms;

    auto roomCount = randRange(params.minRoomCount, params.maxRoomCount, randomEngine);

    rooms.reserve(roomCount);

    rooms.emplace_back(generateRoom(builder, params, randomEngine));
    for (int i = 0; i < roomCount - 1; ++i) {
        rooms.emplace_back(generateRoom(builder, params, randomEngine));
        if (rooms[i + 1].w == 0 && rooms[i + 1].h == 0) // Failed to place room
            break;
        generatorCorridor(builder, params, randomEngine, rooms[i], rooms[i + 1]);
    }

    placeWalls(builder);

    builder.build(world);
}
