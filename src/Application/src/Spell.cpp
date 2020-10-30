#include "Spell.hpp"
#include "SpellDatabase.hpp"

game::Spell::Spell(game::SpellId id) : id(id), cooldown_duration(g_SpellDatabase.at(id).cooldown), current_cooldown(0ms) {}