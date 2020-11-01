#pragma once

#include <unordered_map>
#include <chrono>

#include "factory/SpellFactory.hpp"

namespace game {

struct SpellData {
    std::chrono::milliseconds cooldown;
};

using SpellDatabase = std::unordered_map<SpellFactory::ID, SpellData>;

// TODO: Better way to provide the spell database
extern SpellDatabase g_SpellDatabase;

// note : avoid free function
auto makeSpellDatabase() -> SpellDatabase;

} // namespace game
