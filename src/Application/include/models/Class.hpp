#pragma once


#include <string>
#include <vector>

#include "factory/SpellFactory.hpp"

namespace game {

struct Class {
    using ID = std::uint8_t;

    ID id;
    std::string name;
    std::string description;

    std::vector<SpellFactory::ID> spells;

    float maxHealth;
    float damage;
};

} // namespace game
