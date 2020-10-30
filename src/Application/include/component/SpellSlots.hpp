#pragma once

#include <optional>

#include "Spell.hpp"

namespace game {

struct SpellSlots {
    std::optional<Spell> spells[4];
};

} // namespace game
