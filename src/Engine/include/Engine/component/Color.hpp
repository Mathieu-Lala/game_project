#pragma once

#include <array>
#include <cstdint>

#include <glm/vec3.hpp>

namespace engine {

struct Color {
    // array of value normalized between 0.0 and 1.0
    std::array<float, 12ul> vertices{};
    std::uint32_t VBO{0u};

    static constexpr auto r(const Color &c) noexcept -> float { return c.vertices[0]; }

    static constexpr auto g(const Color &c) noexcept -> float { return c.vertices[1]; }

    static constexpr auto b(const Color &c) noexcept -> float { return c.vertices[2]; }

    // note : you should not call this function yourself // see @DrawableFactory::fix_color
    static auto ctor(glm::vec3 &&color) -> Color;

    // note : you should not call this function yourself // see @DrawableFactory::fix_color
    static auto dtor(Color *color) -> void;
};

} // namespace engine
