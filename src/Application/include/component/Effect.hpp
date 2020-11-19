#pragma once

#include <chrono>
#include <iostream>

namespace game {

struct Effect { // todo : split me in severals components
    bool is_in_effect;
    bool is_in_cooldown;
    std::string effect_name; // todo : use enum

    std::chrono::milliseconds time_effect;
    std::chrono::milliseconds remaining_time_effect;
    std::chrono::milliseconds cooldown;
    std::chrono::milliseconds remaining_cooldown;
};

} // namespace game
