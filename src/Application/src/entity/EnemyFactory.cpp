#include <Engine/helpers/DrawableFactory.hpp>

#include "component/all.hpp"
#include "entity/EnemyFactory.hpp"
#include "EntityDepth.hpp"

#include "Declaration.hpp"

using namespace std::chrono_literals; // ms ..

auto game::EnemyFactory::FirstEnemy(entt::registry &world, const glm::vec2 &pos) -> void
{
    const auto e = world.create();
    world.emplace<entt::tag<"enemy"_hs>>(e);
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::ENEMIES));
    world.emplace<engine::d2::Velocity>(e, 0.02 * (std::rand() & 1), 0.02 * (std::rand() & 1));
    world.emplace<engine::d2::Scale>(e, 1.0, 1.0);
    world.emplace<engine::d2::HitboxSolid>(e, 1.0, 1.0);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 0, 0});
    engine::DrawableFactory::fix_texture(world, e, DATA_DIR "textures/enemy.png");
    world.emplace<ViewRange>(e, 10.0f);
    world.emplace<AttackRange>(e, 3.0f);
    world.emplace<AttackCooldown>(e, false, 4000ms, 0ms);
    world.emplace<AttackDamage>(e, 20.0f);
    world.emplace<Health>(e, 50.0f, 50.0f);
}

auto game::EnemyFactory::Boss(entt::registry &world, const glm::vec2 &pos) -> void
{
    const auto e = world.create();
    world.emplace<entt::tag<"boss"_hs>>(e);
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::ENEMIES));
    world.emplace<engine::d2::Velocity>(e, 0.01 * (std::rand() & 1), 0.01 * (std::rand() & 1));
    world.emplace<engine::d2::Scale>(e, 3.0, 3.0);
    world.emplace<engine::d2::HitboxSolid>(e, 3.0, 3.0);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 0, 0});
    engine::DrawableFactory::fix_texture(world, e, DATA_DIR "textures/enemy.png");
    world.emplace<game::ViewRange>(e, 10.0f);
    world.emplace<game::AttackRange>(e, 3.0f);
    world.emplace<game::AttackCooldown>(e, false, 2000ms, 0ms);
    world.emplace<game::Effect>(e, false, false, "bleed", 2000ms, 0ms, 5000ms, 0ms);
    world.emplace<game::AttackDamage>(e, 30.0f);
    world.emplace<Health>(e, 1.0f, 500.0f);
}

auto game::EnemyFactory::Player(entt::registry &world) -> entt::entity
{
    auto player = world.create();
    world.emplace<entt::tag<"player"_hs>>(player);
    world.emplace<engine::d3::Position>(player, 0.0, 0.0, Z_COMPONENT_OF(EntityDepth::PLAYER));
    world.emplace<engine::d2::Velocity>(player, 0.0, 0.0);
    world.emplace<engine::d2::Acceleration>(player, 0.0, 0.0);
    world.emplace<engine::d2::Scale>(player, 1.0, 1.0);
    world.emplace<engine::d2::HitboxSolid>(player, 1.0, 1.0);
    world.emplace<engine::Drawable>(player, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, player, {0, 0, 1});
    engine::DrawableFactory::fix_texture(world, player, DATA_DIR "textures/player.jpeg");
    world.emplace<Health>(player, 100.0f, 100.0f);
    world.emplace<AttackCooldown>(player, false, 1000ms, 0ms);
    world.emplace<AttackDamage>(player, 50.0f);
    world.emplace<Level>(player, 0u, 0u, 10u);
    return player;
}
