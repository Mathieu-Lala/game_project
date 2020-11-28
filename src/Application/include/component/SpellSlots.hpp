#pragma once

#include <optional>
#include <array>
//#include <string>

//#include "models/Spell.hpp"

namespace game {

struct Spell;

struct SpellSlots {
    std::array<std::optional<Spell>, 4ul> spells;

    // note : should be a reference to the spell castable and not the real spell
    //std::array<std::optional<std::string>, 4ul> spells;
};

} // namespace game
