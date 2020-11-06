#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/Event/Event.hpp>
#include <Engine/Settings.hpp>
#include <Engine/audio/AudioManager.hpp>
#include <Engine/Core.hpp>

#include "DataConfigLoader.hpp"
#include "component/all.hpp"
#include "factory/EntityFactory.hpp"

using namespace std::chrono_literals; // ms ..

auto game::DataConfigLoader::loadClassDatabase(const std::string_view path) -> classes::Database
{
    spdlog::info("Loading class database file: '{}'", path.data());

    std::ifstream file(path.data());

    if (!file.is_open()) { spdlog::error("Can't open the given file"); }
    nlohmann::json jsonData = nlohmann::json::parse(file);


    classes::Database database;

    for (Class::ID id = 0; const auto &[name, data] : jsonData.items()) {
        std::vector<SpellFactory::ID> spells;
        for (const auto &spell : data["spells"]) spells.push_back(static_cast<SpellFactory::ID>(spell.get<int>()));

        database[id] = Class{
            .id = id,
            .name = name,
            .description = data["desc"].get<std::string>(),
            .iconPath = data["icon"],
            .spells = spells,

            .maxHealth = data["maxHealth"].get<float>(),
            .damage = data["damage"].get<float>(),
            .childrenClass = {},
        };

        id++;
    }

    for (auto &[id, dbClass] : database) {
        for (const auto &child : jsonData[dbClass.name]["childrenClass"]) {
            auto c = classes::getByName(database, child);

            if (!c)
                spdlog::warn("Unknown class '{}'. Ignoring", child);
            else
                dbClass.childrenClass.push_back(c.value()->id);
        }
    }

    return database;
}
