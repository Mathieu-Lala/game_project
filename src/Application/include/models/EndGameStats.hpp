#pragma once

#include <cstdint>

#include <entt/entt.hpp>

namespace game {

struct EndGameStats {
    EndGameStats(entt::registry &world, const entt::entity player, double gameTime);

    std::uint32_t finalLevel;
    int enemyKilled;
    double gameTimeInSeconds;
};

} // namespace game
