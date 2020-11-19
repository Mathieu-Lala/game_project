#pragma once

#include <chrono>

#include "factory/SpellFactory.hpp"
#include "models/SpellDatabase.hpp"

#include <Engine/component/Cooldown.hpp>

using namespace std::chrono_literals;

namespace game {

struct Spell {
    SpellFactory::ID id;

    engine::Cooldown cd;

    [[nodiscard]] static auto create(SpellFactory::ID spell) -> Spell
    {
        return {
            .id = spell,
            .cd = {
                .is_in_cooldown = false,
                .cooldown = g_SpellDatabase.at(spell).cooldown,
                .remaining_cooldown = 0ms,
            }};
    }

}; // namespace game

} // namespace game
