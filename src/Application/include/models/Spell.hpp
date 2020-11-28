#pragma once

#include <chrono>
#include <string_view>

#include <nlohmann/json.hpp>

#include <Engine/component/Cooldown.hpp>

#include "factory/SpellFactory.hpp"

using namespace std::chrono_literals;

namespace game {

struct /*[[deprecated]]*/ Spell {
//    SpellFactory::ID id;
    std::string_view id;

    engine::Cooldown cd;
};

struct SpellData {
    std::chrono::milliseconds cooldown;
};

inline void to_json(nlohmann::json &j, const SpellData &spell)
{
    // clang-format off
    j = nlohmann::json{{
        "cooldown", spell.cooldown.count()
    }};
    // clang-format on
}

inline void from_json(const nlohmann::json &j, SpellData &spell)
{
    spell.cooldown = std::chrono::milliseconds{j.at("cooldown")};
}

struct SpellDatabase {
    std::unordered_map<std::string, SpellData> db;

    auto fromFile(const std::string_view) -> bool;

    [[nodiscard]] auto instantiate(const std::string_view) -> std::optional<Spell>;
};

} // namespace game
