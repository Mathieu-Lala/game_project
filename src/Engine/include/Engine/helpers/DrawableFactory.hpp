#pragma once

#include <glm/vec3.hpp>

namespace engine {

class Drawable;

struct DrawableFactory {
    // todo : isolate color in a component ?
    static auto rectangle(glm::vec3 &&color) -> Drawable;
};

} // namespace engine
