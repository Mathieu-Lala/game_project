#include "models/SpellDatabase.hpp"

using namespace std::chrono_literals;

namespace game {

SpellDatabase g_SpellDatabase = makeSpellDatabase();

auto makeSpellDatabase() -> SpellDatabase
{
    SpellDatabase db;

    db[SpellFactory::ID::SHOVEL_ATTACK] = {500ms};
    db[SpellFactory::ID::SWORD_ATTACK] = {500ms};
    db[SpellFactory::ID::FIREBALL] = {1250ms};
    db[SpellFactory::ID::ENEMY_ATTACK] = {1000ms};
    db[SpellFactory::ID::PIERCING_ARROW] = {1000ms};

    return db;
}

} // namespace game

