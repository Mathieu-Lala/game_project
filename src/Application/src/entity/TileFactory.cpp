#include <Engine/Graphics/Shader.hpp>
#include <Engine/component/Drawable.hpp>
#include <Engine/Event/Event.hpp>
#include <Engine/Settings.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/audio/AudioManager.hpp>
#include <Engine/Core.hpp>

#include "entity/TileFactory.hpp"

#include "Engine/component/Position.hpp"
#include "Engine/component/Scale.hpp"

#include "Engine/component/Hitbox.hpp"

#include "EntityDepth.hpp"

auto game::TileFactory::FloorNormalRoom(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> void
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/floor.jpg");
    world.emplace<entt::tag<"terrain"_hs>>(e);
}

auto game::TileFactory::FloorSpawnRoom(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> void
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {0.5, 1, 0.5});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/spawn.png");
    world.emplace<entt::tag<"terrain"_hs>>(e);
}

auto game::TileFactory::FloorBossRoom(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> void
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/floor_boss.jpg");
    world.emplace<entt::tag<"terrain"_hs>>(e);
}

auto game::TileFactory::FloorCorridor(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> void
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/corridor.jpg");
    world.emplace<entt::tag<"terrain"_hs>>(e);
}

auto game::TileFactory::ExitDoor(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size, float) -> void
{
    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {0.75, 0.25, 0.25});
    world.emplace<engine::d2::HitboxSolid>(e, size.x, size.y);
    world.emplace<entt::tag<"terrain"_hs>>(e);
    world.emplace<entt::tag<"exit_door"_hs>>(e);
}

auto game::TileFactory::Wall(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> void
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/wall.jpg");
    world.emplace<engine::d2::HitboxSolid>(e, size.x, size.y);
    world.emplace<entt::tag<"terrain"_hs>>(e);
}

auto game::TileFactory::DebugTile(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> void
{
    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::TERRAIN));
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 0});
    world.emplace<entt::tag<"terrain"_hs>>(e);
}
