#pragma once


#include <string>
#include <vector>

#include "factory/SpellFactory.hpp"
#include "factory/EntityFactory.hpp"

namespace game {

struct Class {
    EntityFactory::ID id;
    std::string name;
    std::string description;
    std::string iconPath;
    std::string assetGraphPath;

    bool is_starter = false;

    std::vector<SpellFactory::ID> spells;

    float maxHealth;
    float damage;

    std::vector<EntityFactory::ID> childrenClass;
};

} // namespace game
