#include <chrono>
#include <optional>
#include <fstream>

#include <spdlog/spdlog.h>

#include <Engine/component/Cooldown.hpp>

#include "factory/SpellFactory.hpp"
#include "models/Spell.hpp"

using namespace std::chrono_literals;

void game::to_json(nlohmann::json &j, const SpellData &spell)
{
    // clang-format off
    j = nlohmann::json{{
        "cooldown", spell.cooldown.count(),
        "damage", spell.damage,
        "hitbox", {
            "x", spell.hitbox.width,
            "y", spell.hitbox.height
        },
        "scale", {
            "x", spell.scale.x,
            "y", spell.scale.y
        },
        "lifetime", spell.lifetime.count(),
        //"offset_to_source", {
        //    "x": spell.offset_to_source_x,
        //    "y": spell.offset_to_source_y
        //}
        "audio_on_cast", spell.audio_on_cast,
        "animation", spell.animation,
        "speed", spell.speed
    }};
    // clang-format on
}

void game::from_json(const nlohmann::json &j, SpellData &spell) try
{
    spell.cooldown = std::chrono::milliseconds{j.at("cooldown")};
    spell.damage = j.at("damage");
    spell.hitbox.width = j.at("hitbox").at("x");
    spell.hitbox.height = j.at("hitbox").at("y");
    spell.scale.x = j.at("scale").at("x");
    spell.scale.y = j.at("scale").at("y");
    spell.lifetime = std::chrono::milliseconds{j.at("lifetime")};
    spell.audio_on_cast = j.at("audio_on_cast");
    spell.animation = j.at("animation");
    spell.speed = j.at("speed");
    spell.offset_to_source_x = j.at("offset_to_source").at("x");
    spell.offset_to_source_y = j.at("offset_to_source").at("y");
    spell.type = [](const auto &type) {
        decltype(SpellData{}.type) out;
        for (const auto &i : type) {
            if (const auto id = SpellData::toType(i); id != SpellData::Type::ZERO) {
                out[id] = true;
            }
        }
        return out;
    }(j.at("type").get<std::vector<std::string>>());
}
catch (nlohmann::json::exception &e)
{
    spdlog::error("failed: {}", e.what());
}

auto game::SpellDatabase::instantiate(const std::string_view spell) -> std::optional<Spell>
{
    if (const auto found = std::find_if(std::begin(db), std::end(db), [&spell](auto &i) { return i.first == spell; });
        found != std::end(db)) {
        return Spell{
            .id = found->first,
            .cd = {
                .is_in_cooldown = false,
                .cooldown = found->second.cooldown,
                .remaining_cooldown = 0ms,
            }};

    } else {
        spdlog::error("SpellDatabase::instantiate: No such spell '{}'", spell.data());
        return {};
    }
}

auto game::SpellDatabase::fromFile(const std::string_view path) -> bool
{
    std::ifstream file(path.data());
    if (!file.is_open()) {
        spdlog::error("Can't open the given file");
        return false;
    }
    const auto jsonData = nlohmann::json::parse(file);

    this->db = jsonData.get<std::decay_t<decltype(this->db)>>();

    return true;
}
