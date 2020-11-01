#pragma once

#include <chrono>

#include "factory/SpellFactory.hpp"
#include "models/SpellDatabase.hpp"

using namespace std::chrono_literals;

namespace game {

struct Spell {
    SpellFactory::ID id;
    std::chrono::milliseconds cooldown_duration;

    std::chrono::milliseconds current_cooldown;

    [[nodiscard]] static auto create(SpellFactory::ID spell) -> Spell
    {
        return { .id = spell, .cooldown_duration = g_SpellDatabase.at(spell).cooldown, .current_cooldown = 0ms };
    }
};

} // namespace game
