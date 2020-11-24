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
#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/Core.hpp>

#include "DataConfigLoader.hpp"
#include "component/all.hpp"
#include "factory/EntityFactory.hpp"

using namespace std::chrono_literals;

auto game::DataConfigLoader::loadClassDatabase(const std::string_view path) -> classes::Database
{
    spdlog::info("Loading class database file: '{}'", path.data());

    std::ifstream file(path.data());

    if (!file.is_open()) { spdlog::error("Can't open the given file"); }
    nlohmann::json jsonData = nlohmann::json::parse(file);

    classes::Database database;

    for (const auto &[name, data] : jsonData.items()) {
        std::vector<SpellFactory::ID> spells;
        // note : see std::transform
        for (const auto &spell : data["spells"]) { spells.push_back(static_cast<SpellFactory::ID>(spell.get<int>())); }

        const auto currentID = EntityFactory::ID_from_string(name);

        database[currentID] = Class{
            .id = currentID,
            .name = name,
            .description = data["desc"].get<std::string>(),
            .iconPath = data["icon"],
            .assetGraphPath = data["assetGraph"],
            .is_starter = data.value("starter", false),
            .spells = spells,
            .maxHealth = data["maxHealth"].get<float>(),
            .damage = data["damage"].get<float>(),
            .children = {},
        };
    }

    for (auto &[id, dbClass] : database) {
        for (const auto &child : jsonData[dbClass.name]["children"]) {
            if (const auto c = classes::getByName(database, child.get<std::string>()); c) {
                dbClass.children.push_back(c->id);
            } else
                UNLIKELY { spdlog::warn("Unknown class '{}'. Ignoring", child); }
        }
    }

    for (const auto &[id, classes] : database) {
        spdlog::info("[{}]", id);

        spdlog::info(
            "id={} name={} description={} iconPath={} assetGraphPath={} is_starter={} spells={} maxHealth={} damage={} "
            "children={}",
            classes.id,
            classes.name,
            classes.description,
            classes.iconPath,
            classes.assetGraphPath,
            classes.is_starter,
            "",//classes.spells,
            classes.maxHealth,
            classes.damage,
            ""//,classes.children
        );
    }

    return database;
}
