#include <chrono>
#include <spdlog/spdlog.h>

#include <glm/gtx/vector_angle.hpp>

#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/component/Rotation.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Core.hpp>

#include "models/Spell.hpp"

#include "component/all.hpp"
#include "factory/SpellFactory.hpp"

using namespace std::chrono_literals;

auto game::SpellFactory::create(entt::registry &world, entt::entity caster, const glm::dvec2 &direction, const SpellData &data)
    -> entt::entity
{
    spdlog::trace("Casting a spell");

    static auto holder = engine::Core::Holder{};
    holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + data.audio_on_cast)->play();

    const auto &caster_pos = world.get<engine::d3::Position>(caster);

    const auto spell = world.create();
    world.emplace<entt::tag<"spell"_hs>>(spell);
    world.emplace<engine::Source>(spell, caster);
    world.emplace<engine::Drawable>(spell, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, spell, glm::vec3{1, 1, 1}); // todo : add color in db ?
    world.emplace<engine::d3::Position>(
        spell,
        caster_pos.x + (data.scale.x / 3.0 + data.offset_to_source_x) * direction.x,
        caster_pos.y + (data.scale.y / 3.0 + data.offset_to_source_y) * direction.y,
        -1.0);
    world.emplace<engine::d2::Rotation>(spell, glm::acos(glm::dot({1.f, 0.f}, direction)));

    world.emplace<engine::d2::Scale>(spell, data.scale);
    world.emplace<engine::d2::HitboxFloat>(spell, data.hitbox);
    world.emplace<game::AttackDamage>(spell, data.damage);
    world.emplace<engine::Lifetime>(spell, data.lifetime);

    world.emplace<engine::Spritesheet>(
        spell, engine::Spritesheet::from_json(holder.instance->settings().data_folder + data.animation));
    engine::DrawableFactory::fix_spritesheet(world, spell, "default");
    world.emplace<engine::d2::Velocity>(spell, direction.x * data.speed, direction.y * data.speed);

    world.emplace<SpellEffect>(spell, data.effects);

    if (data.type[SpellData::Type::PROJECTILE]) world.emplace<entt::tag<"projectile"_hs>>(spell);
    if (data.type[SpellData::Type::AOE]) world.emplace<entt::tag<"aoe"_hs>>(spell);

    return spell;
}
