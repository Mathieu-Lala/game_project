#include "Engine/helpers/DrawableFactory.hpp"

auto engine::DrawableFactory::rectangle([[maybe_unused]] glm::vec3 &&c, Drawable &out) -> void
{
    const float vertices[] = {
        // positions        // color
        -0.5f, -0.5f, 0.0f, c.r, c.g, c.b, // top left
        +0.5f, -0.5f, 0.0f, c.r, c.g, c.b, // top right
        -0.5f, +0.5f, 0.0f, c.r, c.g, c.b, // bottom left
        +0.5f, +0.5f, 0.0f, c.r, c.g, c.b, // bottom right
    };
    static constexpr auto STRIDE_COUNT = 6.0f; // = x + y + z + r + g + b

    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

    // static constexpr auto indices = std::to_array({ 0u, 1u, 2u, 1u, 2u, 3u });
    unsigned int indices[] = {
        0, 1, 2, // first triangle
        1, 2, 3, // second triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE_COUNT * sizeof(float), static_cast<void *>(0));
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE_COUNT * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    out.VAO = VAO;
    out.VBO = VBO;
    out.EBO = EBO;
    out.triangle_count = 2;
}
