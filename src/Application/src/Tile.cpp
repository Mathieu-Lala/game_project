#include "Terrain/Tile.hpp"
#include "Engine/helpers/DrawableFactory.hpp"

Tile::Tile(entt::registry &world, glm::vec2 coord, glm::vec3 color) :
    _simpleColorShader(engine::Shader::CreateFromFiles(
        DATA_DIR "/shaders/simpleColor/simpleColorVertex.glsl",
        DATA_DIR "/shaders/simpleColor/simpleColorFragment.glsl")
    )
{
    auto drawable = createDrawableComponent(coord, color);
    world.emplace<engine::Drawable>(world.create(), drawable);
}

engine::Drawable Tile::createDrawableComponent(glm::vec2 pos, glm::vec3 color)
{
    return ::engine::DrawableFactory::rectange(pos, {0.1f, 0.1f}, color, _simpleColorShader.get());
}
