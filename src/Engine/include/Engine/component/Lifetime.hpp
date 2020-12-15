#pragma once

#include <chrono>

namespace engine {

struct Lifetime {
    std::chrono::milliseconds remaining_lifetime;
};

} // namespace engine
