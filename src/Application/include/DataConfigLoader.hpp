#include <iostream>
#include <fstream>
#include <string_view>

#include <entt/entt.hpp>

#include <Engine/Graphics/Shader.hpp>
#include <../../Application/include/classes/Classes.hpp>

namespace engine {
	struct DataConfigLoader
	{
		static auto loadPlayerConfigFile(const std::string_view &filename, entt::registry &world, entt::entity &player) -> entt::entity;
		static auto loadClassConfigFile(const std::string_view &filename, entt::registry &world, entt::entity &player, engine::Classes cl) -> entt::entity;

		//auto closeAllFiles() -> void;
        auto reloadFiles() -> void;
	};
};