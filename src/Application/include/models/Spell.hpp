#pragma once

#include <chrono>
#include <string_view>
#include <optional>
#include <bitset>

#include <nlohmann/json.hpp>
#include <glm/vec2.hpp>
#include <magic_enum.hpp>

#include <Engine/component/Cooldown.hpp>
#include <Engine/component/Hitbox.hpp>
#include <Engine/component/Scale.hpp>

#include "factory/SpellFactory.hpp"

using namespace std::chrono_literals;

namespace game {

struct /*[[deprecated]]*/ Spell {
    std::string_view id;

    engine::Cooldown cd;
};

struct SpellData {
    std::string name;
    std::string description;
    std::string iconPath;

    std::chrono::milliseconds cooldown;

    float damage;
    engine::d2::HitboxFloat hitbox;
    double speed;

    std::chrono::milliseconds lifetime;
    std::string audio_on_cast;
    std::string animation;
    engine::d2::Scale scale;

    double offset_to_source_x;
    double offset_to_source_y;

    enum Type {
        ZERO = 0,

        PROJECTILE = 1 << 0,
        AOE = 1 << 1,

        TYPE_MAX,
    };

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
        return ZERO;
    }

    std::bitset<TYPE_MAX> type;

    std::vector<std::string> effects;
};

void to_json(nlohmann::json &j, const SpellData &spell);
void from_json(const nlohmann::json &j, SpellData &spell);

struct SpellDatabase {
    std::unordered_map<std::string, SpellData> db;

    auto fromFile(const std::string_view) -> bool;

    [[nodiscard]] auto instantiate(const std::string_view) -> std::optional<Spell>;
};

} // namespace game
