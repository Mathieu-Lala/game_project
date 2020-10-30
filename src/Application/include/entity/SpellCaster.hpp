#pragma once

#include <chrono>

#include <entt/entt.hpp>
#include <Engine/Core.hpp>
#include <spdlog/spdlog.h>
#include <Engine/component/Drawable.hpp>
#include <Engine/component/Position.hpp>
#include <Engine/component/Velocity.hpp>
#include <Engine/component/Scale.hpp>
#include <Engine/component/Hitbox.hpp>
#include <Engine/component/Source.hpp>
#include <Engine/audio/AudioManager.hpp>
#include <Engine/Settings.hpp>
#include <Engine/helpers/DrawableFactory.hpp>

#include "component/AttackDamage.hpp"
#include "component/Lifetime.hpp"

#include "Spellid.hpp"

namespace game {

void castSpell(SpellId spell, entt::registry &world, const entt::entity &caster, const glm::dvec2 &direction);

template<SpellId spell>
void castSpell(entt::registry &world, const entt::entity &caster, const glm::dvec2 &direction);


template<>
void castSpell<SpellId::STICK_ATTACK>(entt::registry &world, const entt::entity &caster, const glm::dvec2 &direction);
template<>
void castSpell<SpellId::SWORD_ATTACK>(entt::registry &world, const entt::entity &caster, const glm::dvec2 &direction);
template<>
void castSpell<SpellId::FIREBALL>(entt::registry &world, const entt::entity &caster, const glm::dvec2 &direction);

} // namespace game


