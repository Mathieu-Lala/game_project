#pragma once

#include <chrono>
#include <string_view>
#include <optional>

#include <nlohmann/json.hpp>
#include <glm/vec2.hpp>

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
};

inline void to_json(nlohmann::json &j, const SpellData &spell)
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

inline void from_json(const nlohmann::json &j, SpellData &spell)
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
}

struct SpellDatabase {
    std::unordered_map<std::string, SpellData> db;

    auto fromFile(const std::string_view) -> bool;

    [[nodiscard]] auto instantiate(const std::string_view) -> std::optional<Spell>;
};

} // namespace game
