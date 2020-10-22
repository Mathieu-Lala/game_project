#include <iostream>
#include <fstream>
#include <string_view>

#include <entt/entt.hpp>

#include <Engine/Shader.hpp>
#include <../../Application/include/classes/Classes.hpp>

namespace engine {
	class DataConfigLoader
	{
    public:
        DataConfigLoader();

		auto loadPlayerConfigFile(const std::string_view &filename, entt::registry &world, entt::entity &player, engine::Shader &shader) -> void;
		auto loadClassConfigFile(const std::string_view &filename, entt::registry &world, entt::entity &player, engine::Classes cl) -> void;

		//auto closeAllFiles() -> void;
        auto reloadFiles() -> void;

    private:

	};
};