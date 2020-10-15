#include <Engine/helpers/DrawableFactory.hpp>

#include "component/all.hpp"
#include "entity/EnemyFactory.hpp"
#include "EntityDepth.hpp"

auto game::EnemyFactory::FirstEnemy(entt::registry &world, engine::Shader *shader, const glm::vec2 &pos) -> void
{
    using namespace std::chrono_literals; // ms ..

    const auto e = world.create();
    world.emplace<entt::tag<"enemy"_hs>>(e);
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::ENEMIES));
    world.emplace<engine::d2::Velocity>(e, 0.02 * (std::rand() & 1), 0.02 * (std::rand() & 1));
    world.emplace<engine::d2::Scale>(e, 1.0, 1.0);
    world.emplace<engine::d2::Hitbox>(e, 1.0, 1.0);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle({1, 0, 0})).shader = shader;
    world.emplace<game::ViewRange>(e, 10.0f);
    world.emplace<game::AttackRange>(e, 3.0f);
    world.emplace<game::AttackCooldown>(e, false, 4000ms, 0ms);
    world.emplace<game::AttackDamage>(e, 20.0f);
}

auto game::EnemyFactory::Boss(entt::registry &world, engine::Shader *shader, const glm::vec2 &pos) -> void
{
    using namespace std::chrono_literals; // ms ..

    const auto e = world.create();
    world.emplace<entt::tag<"boss"_hs>>(e);
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::ENEMIES));
    world.emplace<engine::d2::Velocity>(e, 0.01 * (std::rand() & 1), 0.01 * (std::rand() & 1));
    world.emplace<engine::d2::Scale>(e, 3.0, 3.0);
    world.emplace<engine::d2::Hitbox>(e, 3.0, 3.0);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle({1, 0, 0})).shader = shader;
    world.emplace<game::ViewRange>(e, 10.0f);
    world.emplace<game::AttackRange>(e, 3.0f);
    world.emplace<game::AttackCooldown>(e, false, 2000ms, 0ms);
    world.emplace<game::Effect>(e, false, false, "bleed", 2000ms, 0ms, 5000ms, 0ms);
    world.emplace<game::AttackDamage>(e, 30.0f);
}
