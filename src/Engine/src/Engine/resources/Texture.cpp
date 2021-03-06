#include "Engine/resources/Texture.hpp"

auto engine::Texture::ctor(const std::string_view filepath, bool mirrored_repeated) -> Texture
{
    Texture texture = {
        .id = 0,
        .width = 0,
        .height = 0,
        .channels = 0,
        .px = nullptr,
    };

    texture.px = ::stbi_load(filepath.data(), &texture.width, &texture.height, &texture.channels, 4);
    if (texture.px == nullptr) {
        spdlog::error("Could not open texture '{}'. Texture will appear black", filepath.data());
    }

    CALL_OPEN_GL(::glGenTextures(1, &texture.id));
    CALL_OPEN_GL(::glBindTexture(GL_TEXTURE_2D, texture.id));

    CALL_OPEN_GL(::glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, texture.width, texture.height));
    CALL_OPEN_GL(
        ::glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, texture.px));
    CALL_OPEN_GL(::glGenerateMipmap(GL_TEXTURE_2D));

    if (!mirrored_repeated) {
        CALL_OPEN_GL(::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        CALL_OPEN_GL(::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        CALL_OPEN_GL(::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        CALL_OPEN_GL(::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    } else {

        CALL_OPEN_GL(::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));// GL_MIRRORED_REPEAT
        CALL_OPEN_GL(::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));// GL_MIRRORED_REPEAT

        CALL_OPEN_GL(::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        CALL_OPEN_GL(::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    }
    CALL_OPEN_GL(::glBindTexture(GL_TEXTURE_2D, 0));
    return texture;
}

auto engine::Texture::dtor(Texture *obj) -> void
{
    ::stbi_image_free(obj->px);
    CALL_OPEN_GL(::glDeleteTextures(1, &obj->id));
}
