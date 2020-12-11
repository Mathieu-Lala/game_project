#pragma once

#include <cstdint>
#include <random>
#include <vector>
#include <optional>

#include <entt/entt.hpp>

#include "stage/LevelTilemapBuilder.hpp"

namespace game {

class ThePURGE;

struct Stage {
    struct Room {
        std::int32_t x, y, w, h;

        [[nodiscard]] constexpr bool is_valid(const TilemapBuilder &builder);
    };

    Room spawn{};
    Room boss{};
    std::vector<Room> regularRooms;

    struct Parameters {
        int minRoomSize = 7;  // exluding walls
        int maxRoomSize = 15; // exluding walls

        std::size_t minRoomCount = 5;  // including boss room
        std::size_t maxRoomCount = 10; // including boss room

        int maxDungeonWidth = 50;
        int maxDungeonHeight = 50;

        int minCorridorWidth = 3; // min 3 or player may not fit
        int maxCorridorWidth = 4;

        std::unordered_map<std::string, float> mobDensity{
            {"rock_obstacle", 1.03f},
            {"skeleton", 1.01f},
            {"golem", 5.005f},
            {"zombie", 4.01f},
            {"summoner", 3.005f},
            {"electric_skeleton", 2.005f},
            {"ice_skeleton", 1.01f},
            {"golem_turret", 1.001f},
            {"fire_skeleton", 3.001f}
        };
    };

    auto generate(ThePURGE &, entt::registry &, const Parameters &, std::optional<std::uint32_t> seed = {}) -> Stage &;

    template<std::integral T>
    static auto randRange(T min, T max)
    {
        assert(max > min);

        const auto r = random_engine();
        return min + static_cast<T>(r % static_cast<decltype(r)>(max - min));
    }

    std::uint32_t nextFloorSeed;

    auto clear(entt::registry &, bool kill_the_players) -> void;

private:
    static std::default_random_engine random_engine;

    auto create_floor(ThePURGE &, entt::registry &, const Parameters &);
    auto populate_enemies(ThePURGE &, entt::registry &, const Parameters &);

    // todo : add parameter enemy type
    auto spawn_mob(ThePURGE &, entt::registry &, const Parameters &, const Room &r) -> void;
};

} // namespace game
