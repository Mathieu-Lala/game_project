#include <spdlog/spdlog.h>

#include "Engine/Graphics/third_party.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/component/Drawable.hpp"
#include "Engine/component/Color.hpp"
#include "Engine/helpers/DrawableFactory.hpp"
#include "Engine/Event/Event.hpp"
#include "Engine/Core.hpp"

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

    ::glGenVertexArrays(1, &out.VAO);
    ::glGenBuffers(1, &out.EBO);

    ::glBindVertexArray(out.VAO);

    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out.EBO);
    ::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    ::glGenBuffers(1, &out.VBO);

    ::glBindBuffer(GL_ARRAY_BUFFER, out.VBO);
    ::glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_pos), vertices_pos, GL_STATIC_DRAW);

    ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(0));
    ::glEnableVertexAttribArray(0);

    return out;
}

auto engine::DrawableFactory::fix_color(entt::registry &world, entt::entity e, glm::vec3 &&color) -> Color &
{
    static Core::Holder holder{};

    assert(world.has<Drawable>(e));
    auto &drawable = world.get<Drawable>(e);

    ::glBindVertexArray(drawable.VAO);

    auto handle = holder.instance->getCache<Color>().load<LoaderColor>(
        entt::hashed_string{fmt::format("resource/color/identifier/{}_{}_{}", color.r, color.g, color.b).data()},
        std::move(color));
    if (handle) {
        ::glBindBuffer(GL_ARRAY_BUFFER, handle->VBO);
        ::glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(handle->vertices.size() * sizeof(float)),
            handle->vertices.data(),
            GL_STATIC_DRAW);

        ::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(0));
        ::glEnableVertexAttribArray(1);

        return world.emplace_or_replace<Color>(e, *handle);
    } else {
        spdlog::error("could not load color in cache !");
        throw std::runtime_error("could not load color in cache !");
    }
}
