#include "thepurge_export.h"

#include "models/Spell.hpp"
#include "ThePURGE.hpp"

#define EXTERN_C extern "C"

EXTERN_C THEPURGE_EXPORT auto constructor() -> engine::api::Game *
{
    return new game::ThePURGE{};
}

EXTERN_C THEPURGE_EXPORT auto destructor(engine::api::Game *instance) -> void
{
    delete instance;
}
