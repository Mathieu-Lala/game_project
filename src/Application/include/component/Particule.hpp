#pragma once

namespace game {

struct Particule {
    enum ID {
        HITMARKER,
        NEUTRAL,
        POSITIVE,
        ID_MAX
    };

    ID id;
};

} // namespace
