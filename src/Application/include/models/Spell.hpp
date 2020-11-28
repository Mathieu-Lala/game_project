#pragma once

#include <chrono>

#include <Engine/component/Cooldown.hpp>

#include "factory/SpellFactory.hpp"

using namespace std::chrono_literals;

namespace game {

struct /*[[deprecated]]*/ Spell {
    SpellFactory::ID id;

    engine::Cooldown cd;
};

struct SpellData {
    std::chrono::milliseconds cooldown;
};

struct SpellDatabase {
    std::unordered_map<SpellFactory::ID, SpellData> db;

    auto fromFile(const std::string_view) -> bool;

    [[nodiscard]] auto instantiate(SpellFactory::ID spell) -> Spell;
};

} // namespace game
