#include "SpellDatabase.hpp"

using namespace std::chrono_literals;

namespace game {

SpellDatabase g_SpellDatabase = makeSpellDatabase();

auto makeSpellDatabase() -> SpellDatabase
{
    SpellDatabase db;

    db[SpellId::STICK_ATTACK] = {2000ms};
    db[SpellId::SWORD_ATTACK] = {2000ms};
    db[SpellId::FIREBALL] = {5000ms};

    return db;
}

} // namespace game
