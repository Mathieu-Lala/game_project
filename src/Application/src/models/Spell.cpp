#include <chrono>
#include <optional>
#include <fstream>

#include <spdlog/spdlog.h>

#include <Engine/component/Cooldown.hpp>

#include "factory/SpellFactory.hpp"
#include "models/Spell.hpp"

#include "models/utils.hpp"

using namespace std::chrono_literals;

void game::to_json(nlohmann::json &j, const SpellData &spell)
{
    // clang-format off
    j = nlohmann::json({spell.name, {
        "icon", spell.iconPath,
        "description", spell.description,
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
    }});
    // clang-format on
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

    for (const auto &[name, data] : jsonData.items()) {
        SpellData spell;

        try {
            spell.name = name;
            spell.iconPath = data.at("icon");
            spell.description = data.at("description");
            spell.cooldown = std::chrono::milliseconds{data.at("cooldown")};
            spell.damage = data.at("damage");
            spell.hitbox.width = data.at("hitbox").at("x");
            spell.hitbox.height = data.at("hitbox").at("y");
            spell.scale.x = data.at("scale").at("x");
            spell.scale.y = data.at("scale").at("y");
            spell.lifetime = std::chrono::milliseconds{data.at("lifetime")};
            spell.audio_on_cast = data.at("audio_on_cast");
            spell.animation = data.at("animation");
            spell.speed = data.at("speed");
            spell.offset_to_source_x = data.at("offset_to_source").at("x");
            spell.offset_to_source_y = data.at("offset_to_source").at("y");
            spell.effects = data.value("effect", decltype(spell.effects){});
            spell.type = [](const auto &type) {
                decltype(SpellData{}.type) out;
                for (const auto &i : type) {
                    if (const auto id = toEnum<SpellData::Type>(i); id.has_value()) {
                        out[static_cast<std::size_t>(id.value())] = true;
                    }
                }
                return out;
            }(data.at("type").get<std::vector<std::string>>());
            spell.targets = [](const std::vector<std::string> &in) {
                decltype(SpellData{}.targets) out;
                for (const auto &i : in) {
                    if (const auto id = toEnum<SpellData::Target>(i); id.has_value()) {
                        out[static_cast<std::size_t>(id.value())] = true;
                    }
                }
                return out;
            }(data.value("target", std::vector<std::string>({"enemy"})));
            spell.quantity = data.value("quantity", 1);
            spell.angle = data.value("angle", 0.0f);
        } catch (nlohmann::json::exception &e) {
            spdlog::error("failed: {}", e.what());
            throw; // we probably don't want to continue
        }

        this->db[name] = spell;
    }

    return true;
}
