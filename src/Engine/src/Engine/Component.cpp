#include <spdlog/spdlog.h>

#include "Engine/Graphics/third_party.hpp"

#include "Engine/component/Drawable.hpp"
#include "Engine/component/Color.hpp"

auto engine::Drawable::dtor(const Drawable &drawable) -> void
{
    ::glDeleteVertexArrays(1, &drawable.VAO);
    ::glDeleteBuffers(1, &drawable.VBO);
    ::glDeleteBuffers(1, &drawable.EBO);
}

auto engine::Color::ctor(glm::vec3 &&color) -> Color
{
    Color out = {
        .vertices = {
            color.r, color.g, color.b,
            color.r, color.g, color.b,
            color.r, color.g, color.b,
            color.r, color.g, color.b,
        }};

    ::glGenBuffers(1, &out.VBO);

    return out;
}

auto engine::Color::dtor(Color *color) -> void
{
    ::glDeleteBuffers(1, &color->VBO);
}
