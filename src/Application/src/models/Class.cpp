#include <chrono>
#include <fstream>
#include <algorithm>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <magic_enum.hpp>

#include <Engine/helpers/macro.hpp>

#include "models/Class.hpp"

auto game::ClassDatabase::getByName(const std::string_view name) -> const Class *
{
    if (const auto found = std::find_if(db.begin(), db.end(), [&name](const auto &i) { return i.second.name == name; });
        found != db.end()) {
        return &found->second;
    } else
        UNLIKELY { return nullptr; }
}

auto game::ClassDatabase::getStarterClass() -> const Class &
{
    if (const auto found = std::find_if(db.begin(), db.end(), [](const auto &i) { return i.second.is_starter; });
        found != db.end()) {
        return found->second;
    } else
        UNLIKELY { return db.begin()->second; }
}

using namespace std::chrono_literals;

auto game::ClassDatabase::fromFile(const std::string_view path) -> ClassDatabase &
{
    spdlog::info("Loading class database file: '{}'", path.data());

    std::ifstream file(path.data());
    if (!file.is_open()) { spdlog::error("Can't open the given file"); }
    const auto jsonData = nlohmann::json::parse(file);

    for (const auto &[name, data] : jsonData.items()) {
        std::vector<SpellFactory::ID> spells;
        // note : see std::transform
        // for (const auto &spell : data["spells"]) { spells.push_back(static_cast<SpellFactory::ID>(spell.get<int>())); }

        std::transform(
            std::begin(data["spells"]), std::end(data["spells"]), std::back_inserter(spells), [](const nlohmann::json &i) {
                return static_cast<SpellFactory::ID>(i.get<int>());
            });

        const auto currentID = EntityFactory::toID(name);

        this->db[currentID] = {
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

    for (auto &[id, dbClass] : this->db) {
        for (const auto &child : jsonData[dbClass.name]["children"]) {
            if (const auto c = getByName(child.get<std::string>()); c) {
                dbClass.children.push_back(c->id);
            } else
                UNLIKELY { spdlog::warn("Unknown class '{}'. Ignoring", child); }
        }
    }

    for (const auto &[id, classes] : this->db) {
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
            "", // classes.spells,
            classes.maxHealth,
            classes.damage,
            std::accumulate(std::begin(classes.children), std::end(classes.children), std::string{}, [](auto out, auto &i){ return out + "/" + magic_enum::enum_name(i).data(); })
        );
    }

    return *this;
}
