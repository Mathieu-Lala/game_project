#pragma once


#include <string>
#include <vector>
#include <utility>

#include "factory/SpellFactory.hpp"
#include "factory/EntityFactory.hpp"

#include "engine/component/Hitbox.hpp"

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
    float speed;
    int cost;
    engine::d2::HitboxSolid hitbox;

    std::vector<EntityFactory::ID> children;
};

} // namespace game
