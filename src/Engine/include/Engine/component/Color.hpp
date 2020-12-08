#pragma once

#include <array>
#include <cstdint>

#include <glm/vec4.hpp>

namespace engine {

struct Color {
    std::array<float, 16ul> vertices{};
    std::uint32_t VBO{0u};

    static constexpr auto r(const Color &c) noexcept -> float { return c.vertices[0]; }

    static constexpr auto g(const Color &c) noexcept -> float { return c.vertices[1]; }

    static constexpr auto b(const Color &c) noexcept -> float { return c.vertices[2]; }

    static constexpr auto a(const Color &c) noexcept -> float { return c.vertices[3]; }

    // note : you should not call this function yourself // see @DrawableFactory::fix_color
    static auto ctor(glm::vec4 &&color) -> Color;

    // note : you should not call this function yourself // see @DrawableFactory::fix_color
    static auto dtor(Color *color) -> void;
};

} // namespace engine
