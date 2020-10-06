#include "Engine/helpers/DrawableFactory.hpp"

engine::Drawable engine::DrawableFactory::rectangle(glm::vec2 pos, glm::vec2 size, glm::vec3 c, engine::Shader *shader)
{
    float verticies[] = {
        // positions
        pos.x           , pos.y         , 0.0f, c.r, c.g, c.b, // top left
        pos.x + size.x  , pos.y         , 0.0f, c.r, c.g, c.b, // top right
        pos.x           , pos.y + size.y, 0.0f, c.r, c.g, c.b, // bottom left
        pos.x + size.x  , pos.y + size.y, 0.0f, c.r, c.g, c.b, // bottom right
    };
    unsigned int indices[] = {
        0, 1, 2, // first triangle
        1, 2, 3, // second triangle
    };
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void *>(0));
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return {VBO, VAO, EBO, 2, shader};
}
