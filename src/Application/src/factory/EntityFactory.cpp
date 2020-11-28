#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/component/Rotation.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Core.hpp>

#include "component/all.hpp"
#include "factory/EntityFactory.hpp"

#include "ThePURGE.hpp"

template<>
auto game::EntityFactory::create<game::EntityFactory::FLOOR_NORMAL>(
    ThePURGE &, entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1});
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
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1});
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
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/floor_boss.jpg");
    world.emplace<entt::tag<"terrain"_hs>>(e);
    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::FLOOR_CORRIDOR>(
    ThePURGE &, entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1});
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
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 1});
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
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    // world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    // engine::DrawableFactory::fix_color(world, e, {1, 1, 1});
    // engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/wall.jpg");
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
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 1, 0});
    world.emplace<entt::tag<"terrain"_hs>>(e);
    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::ENEMY>(
    ThePURGE &game, entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<entt::tag<"enemy"_hs>>(e);
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_ENEMY>());
    world.emplace<engine::d2::Velocity>(e, 0.0, 0.0);
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::d2::HitboxSolid>(e, 1.0, 1.0);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 0, 0});
    // engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/enemy.png");
    world.emplace<ViewRange>(e, 10.0f);
    world.emplace<AttackRange>(e, 3.0f);
    world.emplace<AttackDamage>(e, 2.0f);
    world.emplace<Health>(e, 1.0f, 1.0f);

    world.emplace<engine::Spritesheet>(
        e, engine::Spritesheet::from_json(holder.instance->settings().data_folder + "anims/enemies/skeleton/skeleton.data.json"));

    engine::DrawableFactory::fix_spritesheet(world, e, (std::rand() & 1) ? "idle_right" : "idle_left");

    auto &slots = world.emplace<SpellSlots>(e);
    slots.spells[0] = game.dbSpells().instantiate(SpellFactory::ENEMY_ATTACK);

    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::BOSS>(
    ThePURGE &game, entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<entt::tag<"enemy"_hs>>(e);
    world.emplace<entt::tag<"boss"_hs>>(e);
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_ENEMY>());
    world.emplace<engine::d2::Velocity>(e, 0.0, 0.0);
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::d2::HitboxSolid>(e, 1.5, 3.0);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    world.emplace<game::ViewRange>(e, 10.0f);
    world.emplace<game::AttackRange>(e, 5.0f);
    world.emplace<game::AttackCooldown>(e, false, 2000ms, 0ms);
    world.emplace<game::Effect>(e, false, false, "bleed", 2000ms, 0ms, 5000ms, 0ms);
    world.emplace<game::AttackDamage>(e, 1.0f);
    world.emplace<Health>(e, 100.0f, 100.0f);
    engine::DrawableFactory::fix_color(world, e, {0.95f, 0.95f, 0.95f});

    world.emplace<engine::Spritesheet>(
        e, engine::Spritesheet::from_json(holder.instance->settings().data_folder + "anims/bosses/onlyone/boss.data.json"));

    engine::DrawableFactory::fix_spritesheet(world, e, "idle_right");

    [[maybe_unused]] auto &slots = world.emplace<SpellSlots>(e);

    // TODO: actual boss spells
    slots.spells[0] = game.dbSpells().instantiate(SpellFactory::SWORD_ATTACK);
    slots.spells[1] = game.dbSpells().instantiate(SpellFactory::PIERCING_ARROW);

    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::PLAYER>(
    ThePURGE &, entt::registry &world, [[maybe_unused]] const glm::vec2 &pos, [[maybe_unused]] const glm::vec2 &size)
    -> entt::entity
{
    auto player = world.create();

    world.emplace<entt::tag<"player"_hs>>(player);
    world.emplace<engine::d3::Position>(player, 0.0, 0.0, EntityFactory::get_z_layer<EntityFactory::LAYER_PLAYER>());
    world.emplace<engine::d2::Velocity>(player, 0.0, 0.0);
    world.emplace<engine::d2::Rotation>(player, 0.f);
    world.emplace<engine::d2::Acceleration>(player, 0.0, 0.0);
    world.emplace<engine::d2::Scale>(player, 1.5, 2.5);
    world.emplace<engine::d2::HitboxSolid>(player, 0.75, 2.0);
    world.emplace<engine::Drawable>(player, engine::DrawableFactory::rectangle());
    world.emplace<Level>(player, 0u, 0u, 10u);
    world.emplace<KeyPicker>(player);
    world.emplace<SpellSlots>(player);
    world.emplace<Classes>(player);
    world.emplace<SkillPoint>(player, 0);
    world.emplace<ControllerAxis>(player, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f));
    world.emplace<AimSight>(player);
    world.emplace<AimingDirection>(player, glm::vec2(1.f, 0.f));

    engine::DrawableFactory::fix_color(world, player, {1.0f, 1.0f, 1.0f});

    // class dependant, see `GameLogic::apply_class_to_player`
    world.emplace<engine::Spritesheet>(player);
    world.emplace<Health>(player, 1.f, 1.f);
    world.emplace<AttackDamage>(player, 0.f);
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
    world.emplace<engine::d3::Position>(key, pos.x, pos.y, get_z_layer<LAYER_PLAYER>());
    world.emplace<engine::d2::Rotation>(key, 0.f);
    world.emplace<engine::d2::Scale>(key, size.x, size.y);
    world.emplace<engine::Drawable>(key, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, key, {1, 1, 0});
    engine::DrawableFactory::fix_texture(world, key, holder.instance->settings().data_folder + "textures/key.png");
    return key;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::BACKGROUND>(
    ThePURGE &, entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    auto e = world.create();

    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_BACKGROUND>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {0.15, 0.15, 0.15});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/background.jpg");

    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::AIMING_SIGHT>(
    ThePURGE &, entt::registry &world, [[maybe_unused]] const glm::vec2 &, [[maybe_unused]] const glm::vec2 &)
    -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    auto e = world.create();

    world.emplace<engine::d3::Position>(e, 0.0, 0.0, EntityFactory::get_z_layer<EntityFactory::LAYER_AIMING_SIGHT>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, 0.0, 0.0);

    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());

    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/aim_sight.png");

    return e;
}
