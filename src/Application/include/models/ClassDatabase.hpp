#pragma once

#include <unordered_map>
#include <optional>
#include <string>

#include "Class.hpp"

namespace game {

namespace classes {
using Database = std::unordered_map<Class::ID, Class>;

auto getByName(const Database &db, const std::string &name) -> std::optional<const Class *>;

inline auto getStarterClass(const Database &db) -> const Class &
{
    return db.at(0); // First class found in the config file
} 

} // namespace classes

} // namespace game
