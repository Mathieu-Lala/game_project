#include <spdlog/spdlog.h>
#include <stb_image.h>

#include "Engine/Graphics/third_party.hpp"

#include "Engine/component/Drawable.hpp"
#include "Engine/component/Color.hpp"
#include "Engine/component/Texture.hpp"
#include "Engine/resources/LoaderTexture.hpp"

auto engine::Drawable::dtor(const Drawable &drawable) -> void
{
    ::glDeleteVertexArrays(1, &drawable.VAO);
    ::glDeleteBuffers(1, &drawable.VBO);
    ::glDeleteBuffers(1, &drawable.EBO);
}

auto engine::Color::ctor(glm::vec3 &&color) -> Color
{
    // clang-format off
    Color out = {
        .vertices = {
            color.r, color.g, color.b,
            color.r, color.g, color.b,
            color.r, color.g, color.b,
            color.r, color.g, color.b,
        }};
    // clang-format on

    ::glGenBuffers(1, &out.VBO);

    return out;
}

auto engine::Color::dtor(Color *color) -> void { ::glDeleteBuffers(1, &color->VBO); }

auto engine::Texture::ctor(const std::string_view path) -> Texture
{
    Texture out;

    ::glGenBuffers(1, &out.VBO);

    ::glGenTextures(1, &out.texture);
    ::glBindTexture(GL_TEXTURE_2D, out.texture);

    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (out.px = ::stbi_load(path.data(), &out.width, &out.height, &out.channels, 4); !out.px) {
        spdlog::info("failed to load texture", path.data());
    }

    ::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, out.width, out.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, out.px);
    ::glGenerateMipmap(GL_TEXTURE_2D);

    return out;
}

auto engine::Texture::dtor(Texture *ptr) -> void
{
    ::stbi_image_free(ptr->px);
}
