#include <chrono>
#include <spdlog/spdlog.h>

#include <Engine/component/Color.hpp>
#include <Engine/component/Texture.hpp>
#include <Engine/component/Rotation.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Core.hpp>

#include "component/all.hpp"
#include "factory/SpellFactory.hpp"

using namespace std::chrono_literals;

template<>
auto game::SpellFactory::create<game::SpellFactory::SHOVEL_ATTACK>(
    entt::registry &world, entt::entity caster, const glm::dvec2 &direction) -> entt::entity
{
    static engine::Core::Holder holder{};

    spdlog::info("Casting a shovel attack");
    const auto &caster_pos = world.get<engine::d3::Position>(caster);
    const auto &attack_damage = world.get<game::AttackDamage>(caster);

    auto color = world.has<entt::tag<"enemy"_hs>>(caster) ? glm::vec3{0, 1, 0} : glm::vec3{1, 1, 0};

    holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/fire_cast.wav")->play();
    const auto spell = world.create();
    world.emplace<entt::tag<"spell"_hs>>(spell);
    world.emplace<game::Lifetime>(spell, 600ms);
    world.emplace<game::AttackDamage>(spell, attack_damage.damage);
    world.emplace<engine::Drawable>(spell, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, spell, std::move(color));
    world.emplace<engine::d3::Position>(spell, caster_pos.x + direction.x, caster_pos.y + direction.y, -1.0);
    world.emplace<engine::d2::Rotation>(spell, 0.f);
    world.emplace<engine::d2::Scale>(spell, 0.7, 0.7);
    world.emplace<engine::d2::HitboxFloat>(spell, 0.7, 0.7);
    world.emplace<engine::Source>(spell, caster);
    return spell;
}

template<>
auto game::SpellFactory::create<game::SpellFactory::SWORD_ATTACK>(entt::registry &world, entt::entity caster, const glm::dvec2 &direction)
    -> entt::entity
{
    static engine::Core::Holder holder{};

    spdlog::info("Casting a sword attack");

    const auto &caster_pos = world.get<engine::d3::Position>(caster);
    const auto &attack_damage = world.get<game::AttackDamage>(caster);

    auto color = world.has<entt::tag<"enemy"_hs>>(caster) ? glm::vec3{0, 1, 0} : glm::vec3{0.3, 0.7, 0.2};
    
    holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/fire_cast.wav")->play();
    const auto spell = world.create();
    world.emplace<entt::tag<"spell"_hs>>(spell);
    world.emplace<game::Lifetime>(spell, 600ms);
    world.emplace<game::AttackDamage>(spell, attack_damage.damage);
    world.emplace<engine::Drawable>(spell, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, spell, std::move(color));
    world.emplace<engine::d3::Position>(spell, caster_pos.x + direction.x, caster_pos.y + direction.y, -1.0);
    world.emplace<engine::d2::Rotation>(spell, 0.f);
    world.emplace<engine::d2::Scale>(spell, 0.7, 0.7);
    world.emplace<engine::d2::HitboxFloat>(spell, 0.7, 0.7);
    world.emplace<engine::Source>(spell, caster);
    return spell;
}

template<>
auto game::SpellFactory::create<game::SpellFactory::FIREBALL>(entt::registry &world, entt::entity caster, const glm::dvec2 &direction)
    -> entt::entity
{
    static engine::Core::Holder holder{};

    spdlog::info("Casting fireball");

    holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/fire_cast.wav")->play();

    const auto &caster_pos = world.get<engine::d3::Position>(caster);
    const auto &attack_damage = world.get<game::AttackDamage>(caster);

    // note : should be in db.json
    static constexpr auto speed = 5.0;

    const auto spell = world.create();
    world.emplace<entt::tag<"spell"_hs>>(spell);
    world.emplace<game::Lifetime>(spell, 2000ms);
    world.emplace<game::AttackDamage>(spell, attack_damage.damage * 1.5f);
    world.emplace<engine::Drawable>(spell, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, spell, {1, 1, 1});
    auto &sp = world.emplace<engine::Spritesheet>(
        spell,
        engine::Spritesheet::from_json(holder.instance->settings().data_folder + "assets/fireball/fireball.data.json"));
    engine::DrawableFactory::fix_texture(world, spell, holder.instance->settings().data_folder + sp.file);

    world.emplace<engine::d3::Position>(spell, caster_pos.x + direction.x, caster_pos.y + direction.y, -1.0); // note : why -1
    world.emplace<engine::d2::Velocity>(spell, direction.x * speed, direction.y * speed);
    world.emplace<engine::d2::Rotation>(spell, 0.f);
    world.emplace<engine::d2::Scale>(spell, 2.0, 2.0);
    world.emplace<engine::d2::HitboxFloat>(spell, 0.7, 0.7);
    world.emplace<engine::Source>(spell, caster);
    return spell;
}

template<>
auto game::SpellFactory::create<game::SpellFactory::PIERCING_ARROW>(entt::registry &world, entt::entity caster, const glm::dvec2 &direction)
    -> entt::entity
{
    static engine::Core::Holder holder{};

    spdlog::info("Casting a piercing arrow");

    holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/fire_cast.wav")->play();

    static constexpr auto speed = 6.0;

    const auto &caster_pos = world.get<engine::d3::Position>(caster);
    const auto &attack_damage = world.get<game::AttackDamage>(caster);
    
    const auto spell = world.create();
    world.emplace<entt::tag<"spell"_hs>>(spell);
    world.emplace<game::Lifetime>(spell, 1000ms);
    world.emplace<game::AttackDamage>(spell, attack_damage.damage * 1.25f);
    world.emplace<engine::Drawable>(spell, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, spell, {0.6, 0, 0.3});
    world.emplace<engine::d3::Position>(spell, caster_pos.x + direction.x, caster_pos.y + direction.y, -1.0); // note : why -1
    world.emplace<engine::d2::Velocity>(spell, direction.x * speed , direction.y * speed);
    world.emplace<engine::d2::Rotation>(spell, 0.f);
    world.emplace<engine::d2::Scale>(spell, 0.7, 0.7);
    world.emplace<engine::d2::HitboxFloat>(spell, 0.7, 0.7);
    world.emplace<engine::Source>(spell, caster);
    return spell;
}