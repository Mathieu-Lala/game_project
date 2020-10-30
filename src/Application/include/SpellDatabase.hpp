#pragma once

#include <unordered_map>
#include <chrono>

#include "Spellid.hpp"

namespace game {

struct SpellData {
    std::chrono::milliseconds cooldown;
};

using SpellDatabase = std::unordered_map<SpellId, SpellData>;

// TODO: Better way to provide the spell database
extern SpellDatabase g_SpellDatabase;

auto makeSpellDatabase() -> SpellDatabase;

} // namespace game
