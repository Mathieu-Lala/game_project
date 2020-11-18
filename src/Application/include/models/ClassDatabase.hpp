#pragma once

#include <unordered_map>
#include <optional>
#include <string>

#include <Engine/helpers/macro.hpp>

#include "Class.hpp"

namespace game {

namespace classes {

using Database = std::unordered_map<EntityFactory::ID, Class>;

auto getByName(const Database &db, const std::string_view name) -> const Class *;

inline auto getStarterClass(const Database &db) -> const Class &
{
    if (const auto found = std::find_if(db.begin(), db.end(), [](const auto &i) { return i.second.is_starter; });
        found != db.end()) {
        return found->second;
    } else
        UNLIKELY { return db.begin()->second; }
}

} // namespace classes

} // namespace game
