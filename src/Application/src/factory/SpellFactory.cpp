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

auto game::SpellFactory::create(SpellDatabase &db, entt::registry &world, entt::entity caster, const glm::dvec2 &direction, const SpellData &data)
    -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    spdlog::trace("Casting a spell {}", data.name);

    holder.instance->getAudioManager().getSound(holder.instance->settings().data_folder + data.audio_on_cast)->play();

    const auto &caster_pos = world.get<engine::d3::Position>(caster);

    for (int i = 0; i != data.quantity; i++) {
        const auto spell = world.create();
        world.emplace<entt::tag<"spell"_hs>>(spell);
        world.emplace<engine::Drawable>(spell, engine::DrawableFactory::rectangle());
        engine::DrawableFactory::fix_color(world, spell, {1, 1, 1, 1}); // todo : add color in db ?
        [[maybe_unused]] const auto &pos = world.emplace<engine::d3::Position>(
            spell,
            caster_pos.x + (data.scale.x / 3.0 + data.offset_to_source_x) * direction.x,
            caster_pos.y + (data.scale.y / 3.0 + data.offset_to_source_y) * direction.y,
            -1.0);
        world.emplace<engine::d2::Rotation>(
            spell, glm::acos(glm::dot({1.f, 0.f}, direction)) * (direction.y < 0 ? -1.0 : 1.0) + i * data.angle);

        world.emplace<engine::d2::Scale>(spell, data.scale);
        world.emplace<game::AttackDamage>(spell, data.damage);
        world.emplace<engine::Lifetime>(spell, data.lifetime);

        world.emplace<engine::Spritesheet>(
            spell, engine::Spritesheet::from_json(holder.instance->settings().data_folder + data.animation));
        engine::DrawableFactory::fix_spritesheet(world, spell, "default");

        const auto cross = glm::cross(glm::dvec3(1, 0, 0), glm::dvec3(0, 1, 0));
        const auto matrix_rotation = glm::rotate(glm::dmat4(1.0f), i * data.angle, cross);
        const auto rotated = glm::dvec3(matrix_rotation * glm::dvec4(direction.x, direction.y, 0.0f, 1.0f));

        world.emplace<engine::d2::Velocity>(spell, rotated.x * data.speed, rotated.y * data.speed);

        world.emplace<SpellEffect>(spell, data.effects);
        world.emplace<SpellTarget>(spell, data.targets);

        world.emplace<engine::Source>(spell, caster);

        if (data.type[SpellData::Type::PROJECTILE]) world.emplace<entt::tag<"projectile"_hs>>(spell);
        if (data.type[SpellData::Type::AOE]) world.emplace<entt::tag<"aoe"_hs>>(spell);
        if (data.type[SpellData::Type::ON_DEATH]) {
            world.emplace<entt::tag<"on_death"_hs>>(spell);
            world.emplace<SpellSlots>(spell).spells[0] = db.instantiate(data.on_death);
        }
        if (data.type[SpellData::Type::SUMMONER]) {
            world.emplace<Health>(spell, 1.f, 1.f);
            world.emplace<engine::d2::HitboxSolid>(spell, data.hitbox.width, data.hitbox.height);
            world.emplace<Experience>(spell, 0u);
            world.emplace<entt::tag<"enemy"_hs>>(spell);
        } else {
            world.emplace<engine::d2::HitboxFloat>(spell, data.hitbox);
        }

#ifndef NDEBUG
        auto hitbox_entity = world.create();
        world.emplace<entt::tag<"debug_hitbox"_hs>>(hitbox_entity);
        world.emplace<engine::Source>(hitbox_entity, spell);
        world.emplace<engine::Drawable>(hitbox_entity, engine::DrawableFactory::rectangle());
        world.emplace<engine::d2::Rotation>(hitbox_entity, 0.f);
        world.emplace<engine::d3::Position>(
            hitbox_entity, pos.x, pos.y, EntityFactory::get_z_layer<EntityFactory::Layer::LAYER_DEBUG>());
        world.emplace<engine::d2::Scale>(hitbox_entity, data.hitbox.width, data.hitbox.height);
        engine::DrawableFactory::fix_color(world, hitbox_entity, {1, 1, 1, 0.5});
        engine::DrawableFactory::fix_texture(
            world, hitbox_entity, holder.instance->settings().data_folder + "img/transparent.png");
#endif
    }

    return {};
}
