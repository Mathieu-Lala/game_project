#include "Engine/component/Drawable.hpp"
#include "Engine/Graphics/third_party.hpp"

auto engine::Drawable::dtor(const Drawable &drawable) -> void
{
    ::glDeleteVertexArrays(1, &drawable.VAO);
    ::glDeleteBuffers(1, &drawable.VBO);
    ::glDeleteBuffers(1, &drawable.EBO);
}
