#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <utility>

#include <Engine/component/Hitbox.hpp>

#include "factory/SpellFactory.hpp"
#include "factory/EntityFactory.hpp"

namespace game {

struct Class { // todo : name very confusing
    std::string name;
    std::string description;
    std::string iconPath;
    std::string assetGraphPath;

    bool is_starter = false;

    std::vector<std::string> spells;

    float health;

    float speed;
    int cost;
    engine::d2::HitboxSolid hitbox;

    std::vector<std::string> children;
};

struct ClassDatabase {
    std::vector<Class> db;

    auto fromFile(const std::string_view path) -> ClassDatabase &;

    auto getByName(const std::string_view name) -> const Class *;

    auto getStarterClass() -> const Class &;
};

} // namespace game
