#pragma once

namespace game {

struct Particule {
    enum ID {
        HITMARKER,
        POSITIVE,
        ID_MAX
    };

    ID id;
};

} // namespace
