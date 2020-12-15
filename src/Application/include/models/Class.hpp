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
    std::string iconPath;
    std::string assetGraphPath;

    bool is_starter = false;
    int cost;

    float health;
    float speed;
    engine::d2::HitboxSolid hitbox;

    std::vector<std::string> spells;
    std::vector<std::string> children;
};

struct ClassDatabase {
    std::vector<Class> db;

    auto fromFile(const std::string_view path) -> ClassDatabase &;

    auto getByName(const std::string_view name) -> const Class *;

    auto getStarterClass() -> const Class &;
};

} // namespace game
