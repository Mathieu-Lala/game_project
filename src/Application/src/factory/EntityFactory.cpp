#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/component/Rotation.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Core.hpp>

#include "models/Spell.hpp"

#include "component/all.hpp"
#include "factory/EntityFactory.hpp"

#include "ThePURGE.hpp"

auto game::EntityFactory::create([[maybe_unused]] ThePURGE &game, entt::registry &world, const glm::vec2 &pos, const Enemy &data)
    -> entt::entity
{
    auto holder = engine::Core::Holder{};

    const auto enemy = world.create();

    world.emplace<entt::tag<"enemy"_hs>>(enemy);

    if (data.is_boss) world.emplace<entt::tag<"boss"_hs>>(enemy);

    world.emplace<engine::Drawable>(enemy, engine::DrawableFactory::rectangle());
    world.emplace<engine::d2::Rotation>(enemy, 0.f);
    world.emplace<engine::d3::Position>(enemy, pos.x, pos.y, get_z_layer<Layer::LAYER_ENEMY>());

    world.emplace<engine::d2::Scale>(enemy, data.scale);
    world.emplace<engine::d2::HitboxSolid>(enemy, data.hitbox);

    engine::DrawableFactory::fix_color(world, enemy, glm::vec4{data.color});

    world.emplace<engine::Spritesheet>(
        enemy, engine::Spritesheet::from_json(holder.instance->settings().data_folder + data.asset));

    // note : this does not set animation as wished
    engine::DrawableFactory::fix_spritesheet(world, enemy, (std::rand() & 1) ? "idle_right" : "idle_left");

    world.emplace<engine::d2::Velocity>(enemy, 0.0, 0.0);

    world.emplace<game::ViewRange>(enemy, data.view_range);
    world.emplace<game::AttackRange>(enemy, data.attack_range);
    world.emplace<game::Speed>(enemy, data.speed);

    world.emplace<Health>(enemy, data.health, data.health);
    world.emplace<Experience>(enemy, data.experience);

    auto &slots = world.emplace<SpellSlots>(enemy);
    for (auto i = 0ul; i != data.spells.size(); i++) slots.spells[i] = game.dbSpells().instantiate(data.spells[i]);

#ifndef NDEBUG

    auto hitbox_entity = world.create();
    world.emplace<entt::tag<"debug_hitbox"_hs>>(hitbox_entity);
    world.emplace<engine::Source>(hitbox_entity, enemy);
    world.emplace<engine::Drawable>(hitbox_entity, engine::DrawableFactory::rectangle());
    world.emplace<engine::d2::Rotation>(hitbox_entity, 0.f);
    world.emplace<engine::d3::Position>(hitbox_entity, pos.x, pos.y, get_z_layer<Layer::LAYER_DEBUG>());
    world.emplace<engine::d2::Scale>(hitbox_entity, data.hitbox.width, data.hitbox.height);
    engine::DrawableFactory::fix_color(world, hitbox_entity, {1, 1, 1, 0.5});
    engine::DrawableFactory::fix_texture(
        world, hitbox_entity, holder.instance->settings().data_folder + "textures/transparent.png");

#endif

    return enemy;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::PLAYER>(
    ThePURGE &, entt::registry &world, [[maybe_unused]] const glm::vec2 &pos, [[maybe_unused]] const glm::vec2 &size)
    -> entt::entity
{
    auto player = world.create();

    world.emplace<entt::tag<"player"_hs>>(player);
    world.emplace<engine::d3::Position>(player, 0.0, 0.0, EntityFactory::get_z_layer<Layer::LAYER_PLAYER>());
    world.emplace<engine::d2::Velocity>(player, 0.0, 0.0);
    world.emplace<engine::d2::Rotation>(player, 0.f);
    world.emplace<engine::d2::Acceleration>(player, 0.0, 0.0);
    world.emplace<engine::d2::Scale>(player, 0.7, 1.2);
    world.emplace<engine::Drawable>(player, engine::DrawableFactory::rectangle());
    world.emplace<Level>(player, 0u, 0u, 10u);
    world.emplace<KeyPicker>(player);
    world.emplace<SpellSlots>(player);
    world.emplace<Classes>(player);
    world.emplace<SkillPoint>(player, 0);
    world.emplace<ControllerAxis>(player, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f));
    world.emplace<AimSight>(player);
    world.emplace<AimingDirection>(player, glm::vec2(1.f, 0.f));
    world.emplace<StatsTracking>(player);

    engine::DrawableFactory::fix_color(world, player, {1.0f, 1.0f, 1.0f, 1.0f});

    // class dependant, see `GameLogic::apply_class_to_player`
    world.emplace<engine::Spritesheet>(player);
    world.emplace<Health>(player, 0.f, 0.f);
    world.emplace<AttackDamage>(player, 0.f);
    world.emplace<Speed>(player, 1.f);
    world.emplace<engine::d2::HitboxSolid>(player, 1.0f, 1.0f);
    // --

    return player;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::KEY>(
    ThePURGE &, entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    auto key = world.create();
    world.emplace<entt::tag<"key"_hs>>(key);
    world.emplace<engine::d2::HitboxFloat>(key);
    world.emplace<engine::d3::Position>(key, pos.x, pos.y, get_z_layer<Layer::LAYER_PLAYER>());
    world.emplace<engine::d2::Rotation>(key, 0.f);
    world.emplace<engine::d2::Scale>(key, size.x, size.y);
    world.emplace<engine::Drawable>(key, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, key, {1, 1, 0, 1});
    engine::DrawableFactory::fix_texture(world, key, holder.instance->settings().data_folder + "textures/key.png");
    return key;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::AIMING_SIGHT>(
    ThePURGE &, entt::registry &world, [[maybe_unused]] const glm::vec2 &, [[maybe_unused]] const glm::vec2 &)
    -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    auto e = world.create();

    world.emplace<engine::d3::Position>(e, 0.0, 0.0, EntityFactory::get_z_layer<Layer::PARTICULE>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, 0.0, 0.0);

    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());

    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/aim_sight.png");

    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::FLOOR_NORMAL>(
    ThePURGE &, entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<Layer::LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1, 1});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/floor.jpg");
    world.emplace<entt::tag<"terrain"_hs>>(e);
    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::FLOOR_SPAWN>(
    ThePURGE &, entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<Layer::LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1, 1});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/floor.jpg");
    world.emplace<entt::tag<"terrain"_hs>>(e);
    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::FLOOR_BOSS>(
    ThePURGE &, entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<Layer::LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1, 1});
    engine::DrawableFactory::fix_texture(
        world, e, holder.instance->settings().data_folder + "textures/floor_boss.jpg");
    world.emplace<entt::tag<"terrain"_hs>>(e);
    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::FLOOR_CORRIDOR>(
    ThePURGE &, entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<Layer::LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1, 1});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/corridor.jpg");
    world.emplace<entt::tag<"terrain"_hs>>(e);
    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::EXIT_DOOR>(
    ThePURGE &, entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<Layer::LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1, 1});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/door.png");

    world.emplace<engine::d2::HitboxSolid>(e, size.x, size.y);
    world.emplace<entt::tag<"terrain"_hs>>(e);
    world.emplace<entt::tag<"exit_door"_hs>>(e);
    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::WALL>(
    ThePURGE &, entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<Layer::LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::d2::HitboxSolid>(e, size.x, size.y);
    world.emplace<entt::tag<"terrain"_hs>>(e);
    world.emplace<entt::tag<"wall"_hs>>(e);
    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::DEBUG_TILE>(
    ThePURGE &, entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<Layer::LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 0, 1});
    world.emplace<entt::tag<"terrain"_hs>>(e);
    return e;
}
