#pragma once

#include <optional>

#include "models/Spell.hpp"

namespace game {

struct SpellSlots {
    std::optional<Spell> spells[4];
};

} // namespace game
