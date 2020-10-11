#include <glm/vec3.hpp>
#include <Engine/helpers/DrawableFactory.hpp>

#include "level/TileFactory.hpp"
#include "Declaration.hpp"

#include "Engine/component/Position.hpp"
#include "Engine/component/Scale.hpp"

void TileFactory::Floor(entt::registry &world, engine::Shader *shader, glm::vec2 &&pos, glm::vec2 &&size)
{
    auto e = world.create();
    world.emplace<engine::d2::Position>(e, pos.x, pos.y);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle(glm::vec3(1, 1, 1))).shader = shader;
}

void TileFactory::Wall(entt::registry &world, engine::Shader *shader, glm::vec2 &&pos, glm::vec2 &&size)
{
    auto e = world.create();
    world.emplace<engine::d2::Position>(e, pos.x, pos.y);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle(glm::vec3(0, 0, 0))).shader = shader;
    // TODO: hitbox
}
