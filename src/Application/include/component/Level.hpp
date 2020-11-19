#pragma once

#include <cstdint>

namespace game {

struct Level {
    std::uint32_t current_level;
    std::uint32_t current_xp;
    std::uint32_t xp_require;
};

} // namespace game
