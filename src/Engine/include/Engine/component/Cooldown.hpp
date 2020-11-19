#pragma once

#include <chrono>

namespace engine {

struct Cooldown {
    bool is_in_cooldown;

    std::chrono::milliseconds cooldown;
    std::chrono::milliseconds remaining_cooldown;
};

} // namespace engine
