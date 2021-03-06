#include <spdlog/spdlog.h>
#include <stb_image.h>

#include "Engine/Graphics/third_party.hpp"
#include "Engine/component/Drawable.hpp"
#include "Engine/component/Color.hpp"
#include "Engine/component/VBOTexture.hpp"
#include "Engine/helpers/DrawableFactory.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Event/Event.hpp"        // note : should not require this header here
#include "Engine/audio/AudioManager.hpp" // note : should not require this header here
#include "Engine/Settings.hpp"           // note : should not require this header here
#include "Engine/Core.hpp"
#include "Engine/component/Spritesheet.hpp"

auto engine::DrawableFactory::rectangle() -> Drawable
{
    // clang-format off
    static constexpr float vertices_pos[] = {
        -0.5f, -0.5f, 1.0f, // top left
        +0.5f, -0.5f, 1.0f, // top right
        -0.5f, +0.5f, 1.0f, // bottom left
        +0.5f, +0.5f, 1.0f, // bottom right
    };
    static constexpr std::uint32_t indices[] = {
        0, 1, 2, // first triangle
        1, 2, 3, // second triangle
    };
    // clang-format on

    Drawable out;
    out.triangle_count = 2;

    CALL_OPEN_GL(::glGenVertexArrays(1, &out.VAO));
    CALL_OPEN_GL(::glGenBuffers(1, &out.EBO));

    CALL_OPEN_GL(::glBindVertexArray(out.VAO));

    CALL_OPEN_GL(::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out.EBO));
    CALL_OPEN_GL(::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    CALL_OPEN_GL(::glGenBuffers(1, &out.VBO));

    CALL_OPEN_GL(::glBindBuffer(GL_ARRAY_BUFFER, out.VBO));
    CALL_OPEN_GL(::glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_pos), vertices_pos, GL_STATIC_DRAW));

    CALL_OPEN_GL(::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(0)));
    CALL_OPEN_GL(::glEnableVertexAttribArray(0));

    return out;
}

auto engine::DrawableFactory::fix_color(entt::registry &world, entt::entity e, glm::vec4 &&color) -> Color &
{
    static Core::Holder holder{};

    assert(world.has<Drawable>(e));
    auto &drawable = world.get<Drawable>(e);
    CALL_OPEN_GL(::glBindVertexArray(drawable.VAO));

    auto handle = holder.instance->getCache<Color>().load<LoaderColor>(
        entt::hashed_string{fmt::format("resource/color/identifier/{}_{}_{}_{}", color.r, color.g, color.b, color.a).data()},
        std::move(color));
    if (handle) {
        CALL_OPEN_GL(::glBindBuffer(GL_ARRAY_BUFFER, handle->VBO));
        CALL_OPEN_GL(::glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(handle->vertices.size() * sizeof(float)),
            handle->vertices.data(),
            GL_STATIC_DRAW));

        CALL_OPEN_GL(::glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void *>(0)));
        CALL_OPEN_GL(::glEnableVertexAttribArray(1));

        return world.emplace_or_replace<Color>(e, *handle);
    } else {
        spdlog::error("could not load color in cache !");
        throw std::runtime_error("could not load color in cache !");
    }
}

auto engine::DrawableFactory::fix_texture(
    entt::registry &world,
    entt::entity e,
    const std::string_view filepath,
    bool mirrored_repeated,
    const std::array<float, 4ul> &clip) -> VBOTexture &
{
    static Core::Holder holder{};

    assert(world.has<Drawable>(e));
    auto &drawable = world.get<Drawable>(e);
    CALL_OPEN_GL(::glBindVertexArray(drawable.VAO));

    if (const auto handle = holder.instance->getCache<VBOTexture>().load<LoaderVBOTexture>(
            entt::hashed_string{
                fmt::format("resource/texture/identifier/{}_{}_{}_{}_{}_{}", filepath, mirrored_repeated, clip[0], clip[1], clip[2], clip[3]).data()},
            filepath,
            mirrored_repeated,
            clip);
        !handle) {
        spdlog::error("could not load texture in cache : {}", filepath);
        return *world.try_get<VBOTexture>(e);
    } else {
        CALL_OPEN_GL(::glBindBuffer(GL_ARRAY_BUFFER, handle->VBO));
        CALL_OPEN_GL(::glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(handle->vertices.size() * sizeof(float)),
            handle->vertices.data(),
            GL_STATIC_DRAW));

        CALL_OPEN_GL(::glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void *>(0)));
        CALL_OPEN_GL(::glEnableVertexAttribArray(2));

        return world.emplace_or_replace<VBOTexture>(e, *handle);
    }
}

auto engine::DrawableFactory::fix_spritesheet(entt::registry &world, entt::entity entity, const std::string_view animation)
    -> void
{
    using namespace std::chrono_literals;

    auto &sp = world.get<Spritesheet>(entity);
    engine::Spritesheet::Animation *anim{nullptr};
    try {
        anim = &sp.animations.at(animation.data());
    } catch (...) {
        spdlog::error(
            "could not find animation '{}' in {}",
            animation,
            std::accumulate(std::begin(sp.animations), std::end(sp.animations), std::string{}, [](auto old, auto &i) {
                return old + "%" + i.first;
            }));
        return;
    }

    sp.current_animation = animation;
    sp.cooldown.remaining_cooldown = 0ms;
    sp.cooldown.is_in_cooldown = false;
    sp.cooldown.cooldown = anim->cooldown;
    engine::DrawableFactory::fix_texture(world, entity, Core::Holder{}.instance->settings().data_folder + anim->file);
    sp.current_frame = 0;
}
