#pragma once

#include <glm/vec2.hpp>

namespace game {

// Controller axis current statuses

struct ControllerAxis {
    // If input magnitude is less than the deadzone, the input is considered to be zero
    static constexpr auto kDeadzone = 0.1f;

    glm::vec2 movement;
    glm::vec2 aiming;

    // virtual movement joystick corresponding to keyboard player movement. May not be normalized
    glm::vec2 keyboard_movement; 
};

} // namespace game
