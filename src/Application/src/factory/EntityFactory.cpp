#include <Engine/component/Color.hpp>
#include <Engine/component/Texture.hpp>
#include <Engine/component/Rotation.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Core.hpp>

#include "component/all.hpp"
#include "factory/EntityFactory.hpp"
#include "DataConfigLoader.hpp"

template<>
auto game::EntityFactory::create<game::EntityFactory::FLOOR_NORMAL>(
    entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
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
    entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {0.5, 1, 0.5});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/spawn.png");
    world.emplace<entt::tag<"terrain"_hs>>(e);
    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::FLOOR_BOSS>(
    entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
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
    entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
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
    entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
{
    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {0.75, 0.25, 0.25});
    world.emplace<engine::d2::HitboxSolid>(e, size.x, size.y);
    world.emplace<entt::tag<"terrain"_hs>>(e);
    world.emplace<entt::tag<"exit_door"_hs>>(e);
    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::WALL>(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size)
    -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_TERRAIN>());
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    //world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    //engine::DrawableFactory::fix_color(world, e, {1, 1, 1});
    //engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/wall.jpg");
    world.emplace<engine::d2::HitboxSolid>(e, size.x, size.y);
    world.emplace<entt::tag<"terrain"_hs>>(e);
    world.emplace<entt::tag<"wall"_hs>>(e);
    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::DEBUG_TILE>(
    entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size) -> entt::entity
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
auto game::EntityFactory::create<game::EntityFactory::ENEMY>(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size)
    -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<entt::tag<"enemy"_hs>>(e);
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_ENEMY>());
    world.emplace<engine::d2::Velocity>(e, 0.02 * (std::rand() & 1), 0.02 * (std::rand() & 1));
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::d2::HitboxSolid>(e, 1.0, 1.0);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, e, {1, 0, 0});
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + "textures/enemy.png");
    world.emplace<ViewRange>(e, 10.0f);
    world.emplace<AttackRange>(e, 3.0f);
    world.emplace<AttackDamage>(e, 20.0f);
    world.emplace<Health>(e, 50.0f, 50.0f);

    auto &slots = world.emplace<SpellSlots>(e);
    slots.spells[0] = Spell::create(SpellFactory::STICK_ATTACK);

    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::BOSS>(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size)
    -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    const auto e = world.create();
    world.emplace<entt::tag<"enemy"_hs>>(e);
    world.emplace<entt::tag<"boss"_hs>>(e);
    world.emplace<engine::d3::Position>(e, pos.x, pos.y, get_z_layer<LAYER_ENEMY>());
    world.emplace<engine::d2::Velocity>(e, (std::rand() & 1) ? -0.05 : 0.05, (std::rand() & 1) ? -0.05 : 0.05);
    world.emplace<engine::d2::Rotation>(e, 0.f);
    world.emplace<engine::d2::Scale>(e, size.x, size.y);
    world.emplace<engine::d2::HitboxSolid>(e, 3.0, 3.0);
    world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
    world.emplace<game::ViewRange>(e, 10.0f);
    world.emplace<game::AttackRange>(e, 3.0f);
    world.emplace<game::AttackCooldown>(e, false, 2000ms, 0ms);
    world.emplace<game::Effect>(e, false, false, "bleed", 2000ms, 0ms, 5000ms, 0ms);
    world.emplace<game::AttackDamage>(e, 15.0f);
    world.emplace<Health>(e, 500.0f, 500.0f);
    engine::DrawableFactory::fix_color(world, e, {0.95f, 0.95f, 0.95f});

    // todo : add cache
    auto &sp = world.emplace<engine::Spritesheet>(
        e, engine::Spritesheet::from_json(holder.instance->settings().data_folder + "anims/enemies/boss/example.data.json"));
    engine::DrawableFactory::fix_texture(world, e, holder.instance->settings().data_folder + sp.file);

    [[maybe_unused]] auto &slots = world.emplace<SpellSlots>(e);
    // TODO: add some spells

    return e;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::PLAYER>(
    entt::registry &world, [[maybe_unused]] const glm::vec2 &pos, [[maybe_unused]] const glm::vec2 &size) -> entt::entity
{
    static auto holder = engine::Core::Holder{};

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
    world.emplace<Facing>(player, glm::vec2(1.f, 0.f));

    engine::DrawableFactory::fix_color(world, player, {1.0f, 1.0f, 1.0f});

    // class dependant, see `GameLogic::apply_class_to_player`
    world.emplace<engine::Spritesheet>(player);
    world.emplace<Health>(player, 1.f, 1.f);
    world.emplace<AttackDamage>(player, 0.f);
    // --

    return player;
}

template<>
auto game::EntityFactory::create<game::EntityFactory::KEY>(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size)
    -> entt::entity
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
auto game::EntityFactory::create<game::EntityFactory::BACKGROUND>(entt::registry &world, const glm::vec2 &pos, const glm::vec2 &size)
    -> entt::entity
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
