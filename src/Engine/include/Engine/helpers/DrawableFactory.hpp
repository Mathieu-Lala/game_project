#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Engine/component/Drawable.hpp"

namespace engine {

struct DrawableFactory {

    // todo : isolate color in a component ?
    static
    auto rectangle(glm::vec3 &&color) -> Drawable;

};

} // namespace engine
