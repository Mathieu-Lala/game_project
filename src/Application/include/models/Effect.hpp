#pragma once

#include <string_view>
#include <unordered_map>
#include <chrono>

#include <nlohmann/json.hpp>
#include <magic_enum.hpp>

namespace game {

struct Effect {
    enum Type {
        DOT,

        TYPE_MAX,
    };

    Type type;

    static auto toType(std::string in) noexcept -> Type
    {
        const auto to_lower = [](auto str) {
            std::transform(str.begin(), str.end(), str.begin(), [](auto c) { return static_cast<char>(std::tolower(c)); });
            return str;
        };

        in = to_lower(in);

        for (const auto &i : magic_enum::enum_values<Type>()) {
            if (in == to_lower(std::string{magic_enum::enum_name(i)})) { return static_cast<Type>(i); }
        }
        return TYPE_MAX;
    }

    float damage;
    std::chrono::milliseconds lifetime;
    std::chrono::milliseconds cooldown;

};

inline void to_json([[maybe_unused]] nlohmann::json &j, [[maybe_unused]] const Effect &effect) {}

inline void from_json(const nlohmann::json &j, Effect &effect) {
    effect.type = Effect::toType(j.at("type"));
    if (effect.type == Effect::DOT) {
        effect.damage = j.at("damage");
        effect.cooldown = std::chrono::milliseconds{j.at("cooldown")};
        effect.lifetime = std::chrono::milliseconds{j.at("lifetime")};
    }
}

struct EffectDatabase {
    std::unordered_map<std::string, Effect> db;

    auto fromFile(const std::string_view path) -> bool;
};

} // namespace game
