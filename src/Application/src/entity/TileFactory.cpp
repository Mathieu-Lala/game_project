#include <Engine/Graphics/Shader.hpp>
#include <Engine/component/Drawable.hpp>
#include <Engine/helpers/DrawableFactory.hpp>

#include "entity/TileFactory.hpp"
#include "Declaration.hpp"

#include "Engine/component/Position.hpp"
#include "Engine/component/Scale.hpp"

#include "Engine/component/Hitbox.hpp"

#include "EntityDepth.hpp"

#include <spdlog/spdlog.h>

auto game::TileFactory::FloorNormalRoom(
    entt::registry &world, engine::Shader *shader, const glm::vec2 &pos, const glm::vec2 &size) -> void
{
    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle(glm::vec3(1, 1, 1))).shader = shader;
    world.emplace<entt::tag<"terrain"_hs>>(e);
}

auto game::TileFactory::FloorSpawnRoom(entt::registry &world, engine::Shader *shader, const glm::vec2 &pos, const glm::vec2 &size)
    -> void
{
    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle(glm::vec3(0.5, 1, 0.5))).shader = shader;
    world.emplace<entt::tag<"terrain"_hs>>(e);
}

auto game::TileFactory::FloorBossRoom(entt::registry &world, engine::Shader *shader, const glm::vec2 &pos, const glm::vec2 &size)
    -> void
{
    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle(glm::vec3(1, 0.5, 0.5))).shader = shader;
    world.emplace<entt::tag<"terrain"_hs>>(e);
}

auto game::TileFactory::FloorCorridor(entt::registry &world, engine::Shader *shader, const glm::vec2 &pos, const glm::vec2 &size)
    -> void
{
    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle(glm::vec3(0.5, 0.5, 0.5))).shader = shader;
    world.emplace<entt::tag<"terrain"_hs>>(e);
}


auto game::TileFactory::ExitDoor(
    entt::registry &world, engine::Shader *shader, const glm::vec2 &pos, const glm::vec2 &size, float rotation)
    -> void
{
    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle(glm::vec3(0.75, 0.25, 0.25))).shader = shader;
    world.emplace<engine::d2::HitboxSolid>(e, size.x, size.y);
    (void)rotation;

    world.emplace<entt::tag<"terrain"_hs>>(e);
    world.emplace<entt::tag<"exit_door"_hs>>(e);
}

auto game::TileFactory::Wall(entt::registry &world, engine::Shader *shader, const glm::vec2 &pos, const glm::vec2 &size)
    -> void
{
    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle(glm::vec3(0, 0, 0))).shader = shader;
    world.emplace<engine::d2::HitboxSolid>(e, size.x, size.y);
    world.emplace<entt::tag<"terrain"_hs>>(e);
}


auto game::TileFactory::DebugTile(entt::registry &world, engine::Shader *shader, const glm::vec2 &pos, const glm::vec2 &size)
    -> void
{
    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle(glm::vec3(1, 1, 0))).shader = shader;
    world.emplace<entt::tag<"terrain"_hs>>(e);
}
