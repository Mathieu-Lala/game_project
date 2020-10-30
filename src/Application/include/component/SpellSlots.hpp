#pragma once

#include <optional>

#include "Spell.hpp"

namespace game {

struct SpellSlots {
    std::optional<Spell> spells[4]; // TODO: check if this gets properly initialized
};

} // namespace game
