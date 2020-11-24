#pragma once

#include <entt/entt.hpp>

#include "ThePURGE.hpp"

namespace game {

struct MainMenu {
    static auto draw(ThePURGE &game, entt::registry &world) -> void
    {
        // todo : load the resource in a coroutine here

        // todo : style because this is not a debug window
        ImGui::Begin("Menu loading", nullptr, ImGuiWindowFlags_NoDecoration);

        if (ImGui::Button("Start the game")) {
            game.logics()->onGameStart.publish(world);
            game.setState(ThePURGE::State::IN_GAME);
        }

        ImGui::End();
    }
};

} // namespace game
