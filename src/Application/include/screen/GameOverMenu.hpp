#pragma once

#include <entt/entt.hpp>

#include "ThePURGE.hpp"

namespace game {

struct GameOverMenu {
    static auto draw(ThePURGE &game, entt::registry &world) -> void
    {
        // todo : style because this is not a debug window
        ImGui::Begin("Menu Game Over", nullptr, ImGuiWindowFlags_NoDecoration);

        if (ImGui::Button("Your are dead !")) {
            for (const auto &i : world.view<entt::tag<"enemy"_hs>>()) { world.destroy(i); }
            for (const auto &i : world.view<entt::tag<"terrain"_hs>>()) { world.destroy(i); }
            for (const auto &i : world.view<entt::tag<"key"_hs>>()) { world.destroy(i); }
            for (const auto &i : world.view<entt::tag<"player"_hs>>()) { world.destroy(i); }
            for (const auto &i : world.view<entt::tag<"spell"_hs>>()) { world.destroy(i); }

            game.setState(ThePURGE::State::LOADING);
        }

        ImGui::End();
    }
};

} // namespace game
