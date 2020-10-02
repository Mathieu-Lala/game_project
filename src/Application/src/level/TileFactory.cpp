#include "level/TileFactory.hpp"
#include <glm/vec3.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Declaration.hpp>

void TileFactory::Floor(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size)
{
    auto e = world.create();

    auto drawable = ::engine::DrawableFactory::rectange(pos, size, glm::vec3(1, 1, 1), getShader());

    world.emplace<engine::Drawable>(world.create(), drawable);
}

void TileFactory::Wall(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size)
{
    auto e = world.create();

    auto drawable = ::engine::DrawableFactory::rectange(pos, size, glm::vec3(0, 0, 0), getShader());

    world.emplace<engine::Drawable>(world.create(), drawable);
    // TODO: hitbox
}

auto TileFactory::getShader() -> engine::Shader *
{
    static auto shaderInstance = engine::Shader::CreateFromFiles(
        DATA_DIR "/shaders/simpleColor/simpleColorVertex.glsl",
        DATA_DIR "/shaders/simpleColor/simpleColorFragment.glsl");

    return shaderInstance.get();
}
