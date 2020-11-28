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

    const auto get_spells = [](const auto &in) {
        std::vector<SpellFactory::ID> spells;
        std::transform(std::begin(in), std::end(in), std::back_inserter(spells), [](const nlohmann::json &i) {
            return static_cast<SpellFactory::ID>(i.get<int>());
        });
        return spells;
    };

    for (const auto &[name, data] : jsonData.items()) {
        const auto currentID = EntityFactory::toID(name);

        Class c{
            .id = currentID,
            .name = name,
            .description = data["desc"].get<std::string>(),
            .iconPath = data["icon"],
            .assetGraphPath = data["assetGraph"],
            .is_starter = data.value("starter", false),
            .spells = get_spells(data["spells"]),
            .maxHealth = data["maxHealth"].get<float>(),
            .damage = data["damage"].get<float>(),
            .speed = data["speed"].get<float>(),
            .cost = data["cost"].get<int>(),
            .hitbox = engine::d2::HitboxSolid{data["hitbox"]["x"].get<double>(), data["hitbox"]["y"].get<double>()},
            .children = {},
        };
        // Note creating the `Class` instance during assignment raises internal compiler error on MSVC
        db[currentID] = c;
    }

    for (auto &[id, i] : this->db) {
        for (const auto &child : jsonData[i.name]["children"]) {
            if (const auto c = getByName(child.get<std::string>()); c) {
                i.children.push_back(c->id);
            } else
                UNLIKELY { spdlog::warn("Unknown class '{}'. Ignoring", child); }
        }
    }

    for (const auto &[id, classes] : this->db) {
        spdlog::info("[{}]", id);

        spdlog::info(
            "\tid={}\n"
            "\tname={}\n"
            "\tdescription={}\n"
            "\ticonPath={}\n"
            "\tassetGraphPath={}\n"
            "\tis_starter={}\n"
            "\tspells={}\n"
            "\tmaxHealth={}\n"
            "\tdamage={}\n"
            "\tspeed={}\n"
            "\tcost={}\n"
            "\thitbox={},{}\n"
            "\tchildren={}\n",
            classes.id,
            classes.name,
            classes.description,
            classes.iconPath,
            classes.assetGraphPath,
            classes.is_starter,
            "", // classes.spells,
            classes.maxHealth,
            classes.damage,
            classes.speed,
            classes.cost,
            classes.hitbox.width,
            classes.hitbox.height,
            std::accumulate(std::begin(classes.children), std::end(classes.children), std::string{}, [](auto out, auto &i) {
                return out + "/" + magic_enum::enum_name(i).data();
            }));
    }

    return *this;
}
