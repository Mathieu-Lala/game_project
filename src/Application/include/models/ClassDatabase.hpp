#pragma once

#include <unordered_map>

#include "Class.hpp"

namespace game {

using ClassDatabase = std::unordered_map<Class::ID, Class>;

constexpr Class::ID kStarterClassId = 0; // First class found in the config file


} // namespace game
