#pragma once

#include "Engine/Shader.hpp"

namespace engine {

struct Drawable {

    unsigned int VBO{ 0 };
    unsigned int VAO{ 0 };
    unsigned int EBO{ 0 };

    int triangle_count;
    engine::Shader *shader;

    static
    auto system(const Drawable &drawable) -> void
    {
        drawable.shader->use();
        ::glBindVertexArray(drawable.VAO);
        ::glDrawElements(GL_TRIANGLES, 3 * drawable.triangle_count, GL_UNSIGNED_INT, 0);
    }
};

} // namespace engine
