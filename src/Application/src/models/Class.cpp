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
    if (const auto found = std::find_if(db.begin(), db.end(), [&name](const auto &i) { return i.name == name; });
        found != db.end()) {
        return &(*found);
    } else
        UNLIKELY { return nullptr; }
}

auto game::ClassDatabase::getStarterClass() -> const Class &
{
    if (const auto found = std::find_if(db.begin(), db.end(), [](const auto &i) { return i.is_starter; });
        found != db.end()) {
        return *found;
    } else
        UNLIKELY { return *db.begin(); }
}

using namespace std::chrono_literals;

auto game::ClassDatabase::fromFile(const std::string_view path) -> ClassDatabase &
{
    spdlog::info("Loading class database file: '{}'", path.data());

    std::ifstream file(path.data());
    if (!file.is_open()) { spdlog::error("Can't open the given file"); }
    const auto jsonData = nlohmann::json::parse(file);

    for (const auto &[name, data] : jsonData.items()) {
        Class c{
            .name = name,
            .description = data["desc"].get<std::string>(),
            .iconPath = data["icon"],
            .assetGraphPath = data["assetGraph"],
            .is_starter = data.value("starter", false),
            .spells = data["spells"].get<std::vector<std::string>>(),
            .health = data["health"].get<float>(),
            .speed = data["speed"].get<float>(),
            .cost = data["cost"].get<int>(),
            .hitbox = engine::d2::HitboxSolid{data["hitbox"]["x"].get<double>(), data["hitbox"]["y"].get<double>()},
            .children = {},
        };
        // Note creating the `Class` instance during assignment raises internal compiler error on MSVC
        // note : is it still the case ?
        db.emplace_back(c);
    }

    for (auto &i : this->db) {
        for (const auto &child : jsonData[i.name]["children"]) {
            if (const auto c = getByName(child.get<std::string>()); c) {
                i.children.push_back(c->name);
            } else
                UNLIKELY { spdlog::warn("Unknown class '{}'. Ignoring", child); }
        }
    }

    for (const auto &classes : this->db) {
        spdlog::info(
            "\tname={}\n"
            "\tdescription={}\n"
            "\ticonPath={}\n"
            "\tassetGraphPath={}\n"
            "\tis_starter={}\n"
            "\tspells={}\n"
            "\thealth={}\n"
            "\tspeed={}\n"
            "\tcost={}\n"
            "\thitbox={},{}\n"
            "\tchildren={}\n",
            classes.name,
            classes.description,
            classes.iconPath,
            classes.assetGraphPath,
            classes.is_starter,
            std::accumulate(
                std::begin(classes.spells),
                std::end(classes.spells),
                std::string{},
                [](auto out, auto &i) { return out + "/" + i; }),
            classes.health,
            classes.speed,
            classes.cost,
            classes.hitbox.width,
            classes.hitbox.height,
            std::accumulate(std::begin(classes.children), std::end(classes.children), std::string{}, [](auto out, auto &i) {
                return out + "/" + i;
            }));
    }

    return *this;
}
