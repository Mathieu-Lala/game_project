#include <chrono>

#include <Engine/component/Cooldown.hpp>

#include "factory/SpellFactory.hpp"
#include "models/Spell.hpp"

using namespace std::chrono_literals;

auto game::SpellDatabase::instantiate(SpellFactory::ID spell) -> Spell
{
    return {
        .id = spell,
        .cd = {
            .is_in_cooldown = false,
            .cooldown = db.at(spell).cooldown,
            .remaining_cooldown = 0ms,
        }};
}

auto game::SpellDatabase::fromFile(const std::string_view) -> bool
{
    this->db[SpellFactory::ID::SHOVEL_ATTACK] = {500ms};
    this->db[SpellFactory::ID::SWORD_ATTACK] = {500ms};
    this->db[SpellFactory::ID::FIREBALL] = {1250ms};
    this->db[SpellFactory::ID::ENEMY_ATTACK] = {1000ms};
    this->db[SpellFactory::ID::PIERCING_ARROW] = {1000ms};

    return true;
}
