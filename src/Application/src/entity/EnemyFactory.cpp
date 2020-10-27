#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Event/Event.hpp> // note : should not require this header here
#include <Engine/Settings.hpp>
#include <Engine/audio/AudioManager.hpp> // note : should not require this header here
#include <Engine/Core.hpp>

#include "EntityDepth.hpp"
#include "component/all.hpp"
#include "entity/EnemyFactory.hpp"
#include "classes/ClassFactory.hpp"

#include "DataConfigLoader.hpp"

using namespace std::chrono_literals; // ms ..

auto game::EnemyFactory::FirstEnemy(entt::registry &world, const glm::vec2 &pos) -> void
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<entt::tag<"enemy"_hs>>(e);
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::ENEMIES));
    world.emplace<engine::d2::Velocity>(e, 0.02 * (std::rand() & 1), 0.02 * (std::rand() & 1));
    world.emplace<engine::d2::Scale>(e, 1.0, 1.0);
    world.emplace<engine::d2::HitboxSolid>(e, 1.0, 1.0);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 0, 0});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/enemy.png");
    world.emplace<ViewRange>(e, 10.0f);
    world.emplace<AttackRange>(e, 3.0f);
    world.emplace<AttackCooldown>(e, false, 4000ms, 0ms);
    world.emplace<AttackDamage>(e, 20.0f);
    world.emplace<Health>(e, 50.0f, 50.0f);
}

auto game::EnemyFactory::Boss(entt::registry &world, const glm::vec2 &pos) -> void
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<entt::tag<"enemy"_hs>>(e);
    world.emplace<entt::tag<"boss"_hs>>(e);
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, Z_COMPONENT_OF(EntityDepth::ENEMIES));
    world.emplace<engine::d2::Velocity>(e, 0.01 * (std::rand() & 1), 0.01 * (std::rand() & 1));
    world.emplace<engine::d2::Scale>(e, 3.0, 3.0);
    world.emplace<engine::d2::HitboxSolid>(e, 3.0, 3.0);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    world.emplace<game::ViewRange>(e, 10.0f);
    world.emplace<game::AttackRange>(e, 3.0f);
    world.emplace<game::AttackCooldown>(e, false, 2000ms, 0ms);
    world.emplace<game::Effect>(e, false, false, "bleed", 2000ms, 0ms, 5000ms, 0ms);
    world.emplace<game::AttackDamage>(e, 15.0f);
    world.emplace<Health>(e, 500.0f, 500.0f);
    engine::DrawableFactory::fix_color(world, e, {0.95f, 0.95f, 0.95f});
    auto &sp = world.emplace<engine::Spritesheet>(
        e, engine::Spritesheet::from_json(holder.instance->settings().data_folder + "assets/example/example.data.json"));
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + sp.file);
}

auto game::EnemyFactory::Player(entt::registry &world) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    auto player = world.create();

    player = DataConfigLoader::loadPlayerConfigFile(
        holder.instance->settings().data_folder + "config/player.json", world, player);
    player = DataConfigLoader::loadClassConfigFile(
        holder.instance->settings().data_folder + "config/classes.json", world, player, Classes::FARMER);

    return player;
}
