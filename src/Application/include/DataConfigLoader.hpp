#include <iostream>
#include <fstream>
#include <string_view>

#include <entt/entt.hpp>

#include "models/SpellDatabase.hpp"
#include "models/ClassDatabase.hpp"

namespace game {

struct DataConfigLoader {
    static auto loadPlayerConfigFile(const std::string_view, entt::registry &, entt::entity &) -> entt::entity;

    static auto loadClassDatabase(const std::string_view path) -> ClassDatabase;

    auto reloadFiles() -> void;
};

} // namespace game
