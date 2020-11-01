#include <chrono>
#include <spdlog/spdlog.h>

#include <Engine/Core.hpp>
#include <Engine/helpers/DrawableFactory.hpp>

#include "component/all.hpp"
#include "factory/SpellFactory.hpp"

using namespace std::chrono_literals;

template<>
auto game::SpellFactory::create<game::SpellFactory::STICK_ATTACK>(
    entt::registry &world, entt::entity caster, const glm::dvec2 &direction) -> entt::entity
{
    static engine::Core::Holder holder{};

    spdlog::info("casting a stick attack");
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
    world.emplace<engine::d3::Position>(spell, caster_pos.x + direction.x / 2.0, caster_pos.y + direction.y / 2.0, -1.0);
    world.emplace<engine::d2::Scale>(spell, 0.7, 0.7);
    world.emplace<engine::d2::HitboxFloat>(spell, 0.7, 0.7);
    world.emplace<engine::Source>(spell, caster);
    return spell;
}

template<>
auto game::SpellFactory::create<game::SpellFactory::SWORD_ATTACK>(entt::registry &, entt::entity, const glm::dvec2 &)
    -> entt::entity
{
    spdlog::info("casting a sword attack");
    return {};
}

template<>
auto game::SpellFactory::create<game::SpellFactory::FIREBALL>(entt::registry &world, entt::entity caster, const glm::dvec2 &direction)
    -> entt::entity
{
    static engine::Core::Holder holder{};

    spdlog::info("casting fireball");

    holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/fire_cast.wav")->play();

    const auto &caster_pos = world.get<engine::d3::Position>(caster);

    const auto spell = world.create();
    world.emplace<entt::tag<"spell"_hs>>(spell);
    world.emplace<game::Lifetime>(spell, 2000ms);
    world.emplace<game::AttackDamage>(spell, 15.0f);
    world.emplace<engine::Drawable>(spell, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, spell, {0.6, 0.6, 1});
    world.emplace<engine::d3::Position>(spell, caster_pos.x + direction.x / 2.0, caster_pos.y + direction.y / 2.0, -1.0);
    world.emplace<engine::d2::Velocity>(spell, direction.x * 2.0, direction.y * 2.0);
    world.emplace<engine::d2::Scale>(spell, 0.7, 0.7);
    world.emplace<engine::d2::HitboxSolid>(spell, 0.7, 0.7);
    world.emplace<engine::Source>(spell, caster);
    spdlog::info("done");
    return spell;
}
