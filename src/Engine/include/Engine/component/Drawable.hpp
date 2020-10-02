#pragma once

#include "Engine/Shader.hpp"

namespace engine {

struct Drawable {

    unsigned int VBO{ 0 };
    unsigned int VAO{ 0 };
    unsigned int EBO{ 0 };

    int triangle_count;
    engine::Shader *shader;

    void draw() {
        shader->use();

        ::glBindVertexArray(VAO);
        ::glDrawElements(GL_TRIANGLES, 3 * triangle_count, GL_UNSIGNED_INT, 0);
    }
};

} // namespace engine
