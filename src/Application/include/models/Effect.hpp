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
        DASH,

        TYPE_MAX,
    };

    Type type;

    enum Target {
        ENEMY,
        CASTER,

        TARGET_MAX,
    };

    std::vector<Target> targets;

    template<typename T>
    static auto toType(std::string in) noexcept -> std::optional<T>
    {
        const auto to_lower = [](auto str) {
            std::transform(str.begin(), str.end(), str.begin(), [](auto c) { return static_cast<char>(std::tolower(c)); });
            return str;
        };

        in = to_lower(in);

        for (const auto &i : magic_enum::enum_values<T>()) {
            if (in == to_lower(std::string{magic_enum::enum_name(i)})) { return static_cast<T>(i); }
        }
        return {};
    }

    float damage;
    std::chrono::milliseconds lifetime;
    std::chrono::milliseconds cooldown;

    float strength;
};

inline void to_json([[maybe_unused]] nlohmann::json &j, [[maybe_unused]] const Effect &effect) {}

inline void from_json(const nlohmann::json &j, Effect &effect)
{
    effect.type = Effect::toType<Effect::Type>(j.at("type")).value_or(Effect::TYPE_MAX);
    effect.targets = [](const std::vector<std::string> &in) {
        std::vector<Effect::Target> out;
        for (const auto &i : in) {
            if (const auto t = Effect::toType<Effect::Target>(i); t.has_value()) { out.emplace_back(t.value()); }
        }
        return out;
    }(j.value("target", std::vector<std::string>({"enemy"})));
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
