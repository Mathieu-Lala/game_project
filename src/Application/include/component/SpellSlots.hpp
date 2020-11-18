#pragma once

#include <optional>
#include <array>

#include "models/Spell.hpp"

namespace game {

struct SpellSlots {
    std::array<std::optional<Spell>, 4ul> spells;

    inline void removeElem(int pos) { spells[pos] = {}; }
};

} // namespace game
