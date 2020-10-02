#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "Engine/component/Drawable.hpp"

namespace engine::DrawableFactory {

::engine::Drawable rectange(::glm::vec2 pos, ::glm::vec2 size, ::glm::vec3 color, ::engine::Shader *shader);

}