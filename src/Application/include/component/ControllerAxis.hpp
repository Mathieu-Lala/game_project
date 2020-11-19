#pragma once

#include <glm/vec2.hpp>

namespace game {

// Controller axis current statuses

struct ControllerAxis {
    glm::vec2 movement;
    glm::vec2 aiming;
};

} // namespace game
