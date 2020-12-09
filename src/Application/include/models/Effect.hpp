#pragma once

#include <string_view>
#include <unordered_map>
#include <chrono>

#include <nlohmann/json.hpp>

#include "models/utils.hpp"

namespace game {

struct Effect {
    enum Type {
        DOT,
        DASH,

        TYPE_MAX,
    };

    Type type;

    float damage;
    std::chrono::milliseconds lifetime;
    std::chrono::milliseconds cooldown;

    float strength;
};

inline void to_json([[maybe_unused]] nlohmann::json &j, [[maybe_unused]] const Effect &effect) {}

inline void from_json(const nlohmann::json &j, Effect &effect)
{
    effect.type = toEnum<Effect::Type>(j.at("type")).value_or(Effect::TYPE_MAX);
    if (effect.type == Effect::DOT) {
        effect.damage = j.at("damage");
        effect.cooldown = std::chrono::milliseconds{j.at("cooldown")};
        effect.lifetime = std::chrono::milliseconds{j.at("lifetime")};
    } else if (effect.type == Effect::DASH) {
        effect.lifetime = std::chrono::milliseconds{j.at("lifetime")};
        effect.cooldown = std::chrono::milliseconds{j.at("cooldown")};
        effect.strength = j.at("strength");
    }
}

struct EffectDatabase {
    std::unordered_map<std::string, Effect> db;

    auto fromFile(const std::string_view path) -> bool;
};

} // namespace game
