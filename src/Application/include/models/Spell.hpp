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

    enum Type : std::uint8_t {
        TYPE_ZERO = 0,

        PROJECTILE = 1 << 0,
        AOE = 1 << 1,
        SUMMONER = 1 << 2,

        TYPE_MAX,
    };

    std::bitset<Type::TYPE_MAX> type;

    std::vector<std::string> effects;


    enum Target : std::uint8_t {
        TARGET_ZERO = 0,

        ENEMY = 1 << 0,
        CASTER = 1 << 1,

        TARGET_MAX,
    };

    std::bitset<Target::TARGET_MAX> targets;

    int quantity;
    double angle;

};

void to_json(nlohmann::json &j, const SpellData &spell);
void from_json(const nlohmann::json &j, SpellData &spell);

struct SpellDatabase {
    std::unordered_map<std::string, SpellData> db;

    auto fromFile(const std::string_view) -> bool;

    [[nodiscard]] auto instantiate(const std::string_view) -> std::optional<Spell>;
};

} // namespace game
