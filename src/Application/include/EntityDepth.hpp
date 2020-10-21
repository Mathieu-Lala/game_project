#pragma once

namespace game {

#define Z_COMPONENT_OF(e) (static_cast<double>(e))

enum EntityDepth {
    PLAYER,
    ENEMIES,
    TERRAIN,
    UTILITIES,
    MAX_VAL
};

} // namespace game
