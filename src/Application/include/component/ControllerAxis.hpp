#pragma once

#include <glm/vec2.hpp>

namespace game {

// Controller axis current statuses

struct ControllerAxis {
    // If input magnitude is less than the deadzone, the input is considered to be zero
    static constexpr auto kDeadzone = 0.1f;

    glm::vec2 movement;
    glm::vec2 aiming;
};

} // namespace game
