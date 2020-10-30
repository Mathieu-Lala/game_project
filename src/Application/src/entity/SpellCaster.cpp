#include "entity/SpellCaster.hpp"
#include <cassert>


void game::castSpell(SpellId spell, entt::registry &world, const entt::entity &caster, const glm::dvec2 &direction)
{
#define MAP_SPELL(t) \
    case game::t: castSpell<game::t>(world, caster, direction); break;

    switch (spell) {
        MAP_SPELL(SpellId::STICK_ATTACK);
        MAP_SPELL(SpellId::SWORD_ATTACK);
        MAP_SPELL(SpellId::FIREBALL);

    default: assert(false && "unknown spell. Did you forget to map the enum value to factory function ?");
    }
}

using namespace std::chrono_literals;

template<>
void game::castSpell<game::SpellId::STICK_ATTACK>(entt::registry &world, const entt::entity &caster, const glm::dvec2 &dir)
{
    static engine::Core::Holder holder{};

    spdlog::info("casting a stick attack");
    auto &caster_pos = world.get<engine::d3::Position>(caster);
    auto &attack_damage = world.get<game::AttackDamage>(caster);

    auto color = world.has<entt::tag<"enemy"_hs>>(caster) ? glm::vec3{0, 1, 0} : glm::vec3{1, 1, 0};

    holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/fire_cast.wav")->play();
    const auto spell = world.create();
    world.emplace<entt::tag<"spell"_hs>>(spell);
    world.emplace<game::Lifetime>(spell, 600ms);
    world.emplace<game::AttackDamage>(spell, attack_damage.damage);
    world.emplace<engine::Drawable>(spell, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, spell, std::move(color));
    world.emplace<engine::d3::Position>(spell, caster_pos.x + dir.x / 2.0, caster_pos.y + dir.y / 2.0, -1.0);
    world.emplace<engine::d2::Scale>(spell, 0.7, 0.7);
    world.emplace<engine::d2::HitboxFloat>(spell, 0.7, 0.7);
    world.emplace<engine::Source>(spell, caster);
}

template<>
void game::castSpell<game::SpellId::SWORD_ATTACK>(entt::registry &, const entt::entity &, const glm::dvec2 &)
{
    spdlog::info("casting a sword attack");
}

template<>
void game::castSpell<game::SpellId::FIREBALL>(entt::registry &world, const entt::entity &caster, const glm::dvec2 &dir)
{
    static engine::Core::Holder holder{};

    spdlog::info("casting fireball");

    holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + "sounds/fire_cast.wav")->play();

    auto &caster_pos = world.get<engine::d3::Position>(caster);

    const auto spell = world.create();
    world.emplace<entt::tag<"spell"_hs>>(spell);
    world.emplace<game::Lifetime>(spell, 2000ms);
    world.emplace<game::AttackDamage>(spell, 15.0f);
    world.emplace<engine::Drawable>(spell, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, spell, {0.6, 0.6, 1});
    world.emplace<engine::d3::Position>(spell, caster_pos.x + dir.x / 2.0, caster_pos.y + dir.y / 2.0, -1.0);
    world.emplace<engine::d2::Velocity>(spell, dir.x * 2.0, dir.y * 2.0);
    world.emplace<engine::d2::Scale>(spell, 0.7, 0.7);
    world.emplace<engine::d2::HitboxSolid>(spell, 0.7, 0.7);
    world.emplace<engine::Source>(spell, caster);
    spdlog::info("done");
}
