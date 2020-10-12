#include "entity/EnemyFactory.hpp"
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/component/Drawable.hpp>
#include <Engine/component/Position.hpp>
#include <Engine/component/Scale.hpp>
#include <Engine/component/Velocity.hpp>
#include <Engine/component/Acceleration.hpp>
#include <Engine/component/Hitbox.hpp>

#include "component/ViewRange.hpp"
#include "component/AttackRange.hpp"
#include "component/AttackDamage.hpp"
#include "component/AttackCooldown.hpp"
#include "component/Health.hpp"

#include "EntityDepth.hpp"

using namespace std::chrono_literals; // ms ..


void EnemyFactory::FirstEnemy(entt::registry &world, engine::Shader *shader, const glm::vec2 &pos)
{
    auto e = world.create();

    world.emplace<entt::tag<"enemy"_hs>>(e);
    world.emplace<engine::d2::Position>(e, pos.x, pos.y, static_cast<double>(EntityDepth::ENEMIES));
    world.emplace<engine::d2::Velocity>(e, 0.02 * (std::rand() & 1), 0.02 * (std::rand() & 1));
    world.emplace<engine::d2::Scale>(e, 1.0, 1.0);
    world.emplace<engine::d2::Hitbox>(e, 1.0, 1.0);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle({1, 0, 0})).shader = shader;
    world.emplace<game::ViewRange>(e, 10.0f);
    world.emplace<game::AttackRange>(e, 3.0f);
    world.emplace<game::AttackCooldown>(e, false, 4000ms, 0ms);
    world.emplace<game::AttackDamage>(e, 20.0f);
}
