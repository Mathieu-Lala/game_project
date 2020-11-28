#include "models/Stage.hpp"
#include "models/Spell.hpp"

#include "factory/EntityFactory.hpp"

#include "ThePURGE.hpp"

// If gap is even, center will be chosen randomly between the two center tiles
static int getOnePossibleCenterOf(int a, int b)
{
    int result = (a + b) / 2;

    if ((a - b) % 2 == 0) result -= game::Stage::randRange(0, 1);

    return result;
}

// If room size is even, center will be chosen randomly between the two center tiles
static auto getOnePossibleCenterOf(const game::Stage::Room &r) -> glm::ivec2
{
    return {getOnePossibleCenterOf(r.x, r.x + r.w), getOnePossibleCenterOf(r.y, r.y + r.h)};
}

static void generatorCorridor(
    game::TilemapBuilder &builder,
    const game::Stage::Parameters &params,
    const game::Stage::Room &r1,
    const game::Stage::Room &r2)
{
    auto start = getOnePossibleCenterOf(r1);
    auto end = getOnePossibleCenterOf(r2);

    auto vertical = [&](glm::ivec2 pos, int width) -> glm::ivec2 {
        const auto widthOffset = getOnePossibleCenterOf(0, width);

        const auto minX = pos.x - widthOffset;
        const auto maxX = pos.x + (width - widthOffset);

        while (pos.y != end.y) {
            for (auto x = minX; x < maxX; ++x) {
                if (builder.at(glm::ivec2{x, pos.y}) == game::TileEnum::NONE) {
                    builder[glm::ivec2{x, pos.y}] = game::TileEnum::FLOOR_CORRIDOR;
                }
            }

            pos.y += pos.y < end.y ? 1 : -1;
        }

        return pos;
    };

    auto horizontal = [&](glm::ivec2 pos, int width) -> glm::ivec2 {
        const auto widthOffset = getOnePossibleCenterOf(0, width);

        const auto minY = pos.y - widthOffset;
        const auto maxY = pos.y + (width - widthOffset);

        while (pos.x != end.x) {
            for (auto y = minY; y < maxY; ++y) {
                if (builder.at(glm::ivec2{pos.x, y}) == game::TileEnum::NONE) {
                    builder[glm::ivec2{pos.x, y}] = game::TileEnum::FLOOR_CORRIDOR;
                }
            }

            pos.x += pos.x < end.x ? 1 : -1;
        }

        return pos;
    };

    // -2 for walls, and -1 to be safe about the random center not making wall go off bound
    auto maxWidth = std::min(r1.h - 3, r2.w - 3);
    auto width = game::Stage::randRange(
        std::min(maxWidth, params.minCorridorWidth), std::min(maxWidth + 1, params.maxCorridorWidth + 1));

    if (game::Stage::randRange(0, 1)) {
        horizontal(vertical(start, width), width);
    } else {
        vertical(horizontal(start, width), width);
    }
}

static void placeWalls(game::TilemapBuilder &builder)
{
    // For each empty tile, if one it's neighbour is a floor, make it a wall
    static constexpr auto neighbours =
        std::to_array<glm::ivec2>({{-1, -1}, {+0, -1}, {+1, -1}, {-1, +0}, {+1, +0}, {-1, +1}, {+0, +1}, {+1, +1}});

    glm::ivec2 it;

    for (it.y = 0; it.y < builder.getSize().y; ++it.y) {
        for (it.x = 0; it.x < builder.getSize().x; ++it.x) {
            if (builder.at(glm::ivec2{it.x, it.y}) != game::TileEnum::NONE
                && builder.at(glm::ivec2{it.x, it.y}) != game::TileEnum::RESERVED)
                continue;

            for (const auto &n : neighbours) {
                const auto checkPos = it + n;

                if (checkPos.x > 0 && checkPos.x < builder.getSize().x && checkPos.y > 0
                    && checkPos.y < builder.getSize().y && IS_FLOOR(builder.at(glm::ivec2{checkPos.x, checkPos.y}))) {
                    builder[glm::ivec2{it.x, it.y}] = game::TileEnum::WALL;
                    break;
                }
            }
        }
    }
}

static auto placeBossRoomExitDoor(game::TilemapBuilder &builder, game::Stage::Room &r) -> void
{
    // Strategy :
    //  Check room boundaries, find the boss room entrance, place a door at the central opposite of the room with the right orientation
    //  Reduce room size by one to make sure the door is not accessible by another room on the other side of the wall

    const auto x1 = r.x;
    const auto x2 = r.x + r.w;
    const auto y1 = r.y;
    const auto y2 = r.y + r.h;

    // CHECK NORTH WALL
    for (auto x = x1 + 1; x < x2 - 1; ++x)
        if (IS_FLOOR(builder.at(glm::ivec2{x, y2 - 1}))) {
            r.y += 1;
            r.h -= 1;

            builder[glm::ivec2{x2 - 1 - (x - x1), r.y}] = game::TileEnum::EXIT_DOOR_FACING_NORTH;
            return;
        }

    // CHECK SOURTH WALL
    for (auto x = x1 + 1; x < x2 - 1; ++x)
        if (IS_FLOOR(builder.at(glm::ivec2{x, y1}))) {
            r.h -= 1;

            builder[glm::ivec2{x2 - 1 - (x - x1), r.y + r.h - 1}] = game::TileEnum::EXIT_DOOR_FACING_SOUTH;
            return;
        }

    // CHECK WEST WALL
    for (auto y = y1 + 1; y < y2 - 1; ++y)
        if (IS_FLOOR(builder.at(glm::ivec2{x1, y}))) {
            r.w -= 1;

            builder[glm::ivec2{r.x + r.w - 1, y2 - 1 - (y - y1)}] = game::TileEnum::EXIT_DOOR_FACING_WEST;
            return;
        }

    // CHECK EAST WALL
    for (auto y = y1 + 1; y < y2 - 1; ++y)
        if (IS_FLOOR(builder.at(glm::ivec2{x2 - 1, y}))) {
            r.x += 1;
            r.w -= 1;

            builder[glm::ivec2{r.x, y2 - 1 - (y - y1)}] = game::TileEnum::EXIT_DOOR_FACING_EAST;
            return;
        }

    assert(false && "Could not find boss room entrance, cannot place exit door");
}

/////////

constexpr bool game::Stage::Room::is_valid(const TilemapBuilder &builder)
{
    const auto x2 = x + w;
    const auto y2 = y + h;

    for (auto i = x; i < x2; ++i) {
        for (auto j = y; j < y2; ++j) {
            if (builder.at(glm::ivec2{i, j}) != game::TileEnum::NONE) { return false; }
        }
    }
    return true;
}

std::default_random_engine game::Stage::random_engine{};

auto game::Stage::create_floor(ThePURGE &game, entt::registry &world, const Parameters &params)
{
    TilemapBuilder builder(game, {params.maxDungeonWidth, params.maxDungeonHeight});

    const auto generate_room = [&builder](const auto &p) -> Room {
        Room r;

        std::uint32_t tries = 0;
        do {
            if (tries++ > 10000) return {0, 0, 0, 0};

            // Width including walls (hence the +2)
            r.w = randRange(p.minRoomSize + 2, p.maxRoomSize + 2 + 1);
            r.h = randRange(p.minRoomSize + 2, p.maxRoomSize + 2 + 1);

            r.x = randRange(0, p.maxDungeonWidth - r.w);
            r.y = randRange(0, p.maxDungeonHeight - r.h);
        } while (!r.is_valid(builder));

        return r;
    };

    const auto set_room = [&builder](const Room &r, TileEnum tile) -> void {
        const auto x2 = r.x + r.w;
        const auto y2 = r.y + r.h;

        for (auto y = r.y + 1; y < y2 - 1; ++y) {
            for (auto x = r.x + 1; x < x2 - 1; ++x) { builder[glm::ivec2{x, y}] = tile; }
        }
    };

    auto roomCount = randRange(params.minRoomCount, params.maxRoomCount);

    std::vector<Room> rooms;
    rooms.reserve(roomCount);

    rooms.emplace_back(generate_room(params));
    set_room(rooms[0], game::TileEnum::RESERVED);

    for (std::size_t i = 0; i < roomCount - 1; ++i) {
        auto room = generate_room(params);
        if (room.w == 0 && room.h == 0) break; // Failed to place room

        // Reserve the space, so multiple rooms don't spawn at the same place
        set_room(room, game::TileEnum::RESERVED);
        rooms.emplace_back(room);

        generatorCorridor(builder, params, rooms[i], rooms[i + 1]);
    }

    this->spawn = *rooms.begin();
    this->boss = *rooms.rbegin();

    // see std::copy
    for (auto i = 1ul; i < rooms.size() - 1; ++i) this->regularRooms.push_back(rooms[i]);

    set_room(this->spawn, game::TileEnum::FLOOR_SPAWN);
    for (const auto &r : this->regularRooms) set_room(r, game::TileEnum::FLOOR_NORMAL_ROOM);

    placeBossRoomExitDoor(builder, this->boss);
    set_room(this->boss, game::TileEnum::FLOOR_BOSS_ROOM);

    placeWalls(builder);

    builder.build(world);
}

auto game::Stage::spawn_mob(ThePURGE &game, entt::registry &world, const Parameters &params, const Room &r) -> void
{
    for (const auto &[id, density] : params.mobDensity) {
        if (density == 0) continue;

        for (auto x = r.x + 1; x < r.x + r.w - 1; ++x) {
            for (auto y = r.y + 1; y < r.y + r.h - 1; ++y) {
                if (randRange(0, static_cast<int>(1.0f / density)) == 0) {
                    EntityFactory::create(game, world, glm::vec2{x + 0.5, y + 0.5}, game.dbEnemies().db.at(id));
                }
            }
        }
    }
}

auto game::Stage::populate_enemies(ThePURGE &game, entt::registry &world, const Parameters &params)
{
    for (auto &r : regularRooms) spawn_mob(game, world, params, r);

    EntityFactory::create(game, world, glm::vec2{boss.x + boss.w * 0.5, boss.y + boss.h * 0.5}, game.dbEnemies().db.at("dark_skeleton"));

//    EntityFactory::create<EntityFactory::BOSS>(
//        game, world, glm::vec2{boss.x + boss.w * 0.5, boss.y + boss.h * 0.5}, {3.0, 3.0});
}

auto game::Stage::generate(ThePURGE &game, entt::registry &world, const Parameters &params, std::optional<std::uint32_t> seed)
    -> Stage &
{
    this->regularRooms.clear();

    if (seed) random_engine.seed(seed.value());

    create_floor(game, world, params);
    populate_enemies(game, world, params);

    nextFloorSeed = static_cast<std::uint32_t>(random_engine());

    return *this;
}
