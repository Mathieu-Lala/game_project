#include <spdlog/spdlog.h>
#include <stb_image.h>

#include "Engine/Graphics/third_party.hpp"

#include "Engine/component/Drawable.hpp"
#include "Engine/component/Color.hpp"
#include "Engine/component/VBOTexture.hpp"
#include "Engine/resources/LoaderVBOTexture.hpp"

#include "Engine/Core.hpp"

auto engine::Drawable::dtor(const Drawable &drawable) -> void
{
    CALL_OPEN_GL(::glDeleteVertexArrays(1, &drawable.VAO));
    CALL_OPEN_GL(::glDeleteBuffers(1, &drawable.VBO));
    CALL_OPEN_GL(::glDeleteBuffers(1, &drawable.EBO));
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

    CALL_OPEN_GL(::glGenBuffers(1, &out.VBO));

    return out;
}

auto engine::Color::dtor(Color *color) -> void { CALL_OPEN_GL(::glDeleteBuffers(1, &color->VBO)); }

auto engine::VBOTexture::ctor(const std::string_view path, const std::array<float, 4ul> &clip) -> VBOTexture
{
    // clang-format off
    VBOTexture out = {
        .vertices = {
            clip[0],             clip[1] + clip[3],   // top left
            clip[0] + clip[2],   clip[1] + clip[3],   // top right
            clip[0],             clip[1],             // bottom left
            clip[0] + clip[2],   clip[1],             // bottom right
        },
        .VBO = 0,
        .id = 0,
        .mirrored = false,
    };
    // clang-format on

    CALL_OPEN_GL(::glGenBuffers(1, &out.VBO));

    auto handle = Core::Holder{}.instance->getCache<Texture>().load<LoaderTexture>(
        entt::hashed_string{fmt::format("resource/texture/identifier/{}", path.data()).data()}, path);
    if (!handle) {
        spdlog::error("could not load texture in cache !");
        throw std::runtime_error("could not load texture in cache !");
    } else {
        out.id = entt::hashed_string{fmt::format("resource/texture/identifier/{}", path.data()).data()};
    }

    return out;
}

auto engine::VBOTexture::dtor(VBOTexture *ptr) -> void
{
    CALL_OPEN_GL(::glDeleteBuffers(1, &ptr->VBO));
}
