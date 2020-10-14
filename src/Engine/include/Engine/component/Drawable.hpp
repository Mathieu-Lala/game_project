#pragma once

#include "Engine/Shader.hpp"

namespace engine {

struct Drawable {

    unsigned int VBO{ 0 };
    unsigned int VAO{ 0 };
    unsigned int EBO{ 0 };

    int triangle_count;
    engine::Shader *shader;

};

} // namespace engine
