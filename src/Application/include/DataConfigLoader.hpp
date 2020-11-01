#include <iostream>
#include <fstream>
#include <string_view>

#include <entt/entt.hpp>

#include <Engine/Graphics/Shader.hpp>
#include "classes/Classes.hpp"
#include "models/SpellDatabase.hpp"

namespace game {

struct DataConfigLoader {
    static auto loadPlayerConfigFile(const std::string_view, entt::registry &, entt::entity &) -> entt::entity;

    static auto loadClassConfigFile(const std::string_view, entt::registry &, entt::entity &, Classes cl) -> void;

    auto reloadFiles() -> void;
};

} // namespace game
