#pragma once

#include <cstdint>

namespace engine {

class Shader;

// todo : redo me // see @Color component
struct Drawable {
    std::uint32_t VBO{0};
    std::uint32_t VAO{0};
    std::uint32_t EBO{0};

    int triangle_count;

    static auto dtor(const Drawable &drawable) -> void;

};

} // namespace engine
