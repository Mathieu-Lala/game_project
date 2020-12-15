#pragma once

#include <entt/entt.hpp>

#include "ThePURGE.hpp"

namespace game {

namespace widget {

struct TerrainGeneration {
    static auto draw(ThePURGE &game, entt::registry &world) -> void
    {
        static bool spamNextFloor = false;

        ImGui::Begin("MapGeneration");

        ImGui::Checkbox("Spam next floor", &spamNextFloor);

        if (ImGui::Button("Next floor") || spamNextFloor) game.logics()->onFloorChange.publish(world);

        ImGui::SliderInt(
            "Min room size",
            &game.logics()->m_map_generation_params.minRoomSize,
            0,
            game.logics()->m_map_generation_params.maxRoomSize);
        ImGui::SliderInt(
            "Max room size",
            &game.logics()->m_map_generation_params.maxRoomSize,
            game.logics()->m_map_generation_params.minRoomSize,
            50);
        ImGui::Separator();

        ImGui::InputScalar("Min room count", ImGuiDataType_U64, &game.logics()->m_map_generation_params.minRoomCount);
        ImGui::InputScalar("Max room count", ImGuiDataType_U64, &game.logics()->m_map_generation_params.maxRoomCount);
        ImGui::Text("note: actual room count may be smaller if there is not enough space");
        ImGui::Separator();

        ImGui::DragInt("Max dungeon width", &game.logics()->m_map_generation_params.maxDungeonWidth, 0, 500);
        ImGui::DragInt("Max dungeon height", &game.logics()->m_map_generation_params.maxDungeonHeight, 0, 500);
        ImGui::Separator();

        ImGui::SliderInt(
            "Min corridor width",
            &game.logics()->m_map_generation_params.minCorridorWidth,
            0,
            game.logics()->m_map_generation_params.maxCorridorWidth);
        ImGui::SliderInt(
            "Max corridor width",
            &game.logics()->m_map_generation_params.maxCorridorWidth,
            game.logics()->m_map_generation_params.minCorridorWidth,
            50);
        ImGui::Separator();

        for (const auto &[name, _] : game.dbEnemies().db) {
            ImGui::SliderFloat("Enemy '{}' per block", &game.logics()->m_map_generation_params.mobDensity[name], 0, 1);
        }

        ImGui::End();
    }
};

} // namespace widget

} // namespace game
