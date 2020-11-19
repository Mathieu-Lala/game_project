#pragma once

#include <chrono>

namespace game {

// todo : see @Engine/component/cooldown
struct /*[[deprecated]]*/ AttackCooldown {
    bool is_in_cooldown;

    std::chrono::milliseconds cooldown;
    std::chrono::milliseconds remaining_cooldown;
};

} // namespace game
