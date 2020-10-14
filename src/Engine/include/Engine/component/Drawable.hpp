#pragma once

namespace engine {

class Shader;

struct Drawable {
    std::uint32_t VBO{0};
    std::uint32_t VAO{0};
    std::uint32_t EBO{0};

    int triangle_count;
    Shader *shader;
};

} // namespace engine
