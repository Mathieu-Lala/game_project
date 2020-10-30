#pragma once

#include <map>
#include <chrono>

#include "Spellid.hpp"

using namespace std::chrono_literals;

namespace game {

struct Spell {
    explicit Spell(game::SpellId id);

    game::SpellId id;
    std::chrono::milliseconds cooldown_duration;

    std::chrono::milliseconds current_cooldown;
};

};
