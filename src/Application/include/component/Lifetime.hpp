#pragma once

#include <chrono>

namespace game {

struct Lifetime {
    std::chrono::milliseconds remaining_lifetime;
};

} // namespace game
