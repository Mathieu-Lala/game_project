#include "models/SpellDatabase.hpp"

using namespace std::chrono_literals;

namespace game {

SpellDatabase g_SpellDatabase = makeSpellDatabase();

auto makeSpellDatabase() -> SpellDatabase
{
    SpellDatabase db;

    db[SpellFactory::ID::SHOVEL_ATTACK] = {2000ms};
    db[SpellFactory::ID::SWORD_ATTACK] = {2000ms};
    db[SpellFactory::ID::FIREBALL] = {5000ms};
    db[SpellFactory::ID::PIERCING_ARROW] = {4000ms};

    return db;
}

} // namespace game
