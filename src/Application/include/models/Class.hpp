#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

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

    std::vector<EntityFactory::ID> children;
};

struct ClassDatabase {
    std::unordered_map<EntityFactory::ID, Class> db;

    auto fromFile(const std::string_view path) -> ClassDatabase &;

    auto getByName(const std::string_view name) -> const Class *;

    auto getStarterClass() -> const Class &;
};

} // namespace game
