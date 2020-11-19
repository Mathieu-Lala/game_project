#include <iostream>
#include <fstream>
#include <string_view>

#include <entt/entt.hpp>

#include "models/SpellDatabase.hpp"
#include "models/ClassDatabase.hpp"

namespace game {

struct /* [[deprecated]] */ DataConfigLoader {
    static auto loadClassDatabase(const std::string_view path) -> classes::Database;
};

} // namespace game
