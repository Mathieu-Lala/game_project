#pragma once

#include <entt/entt.hpp>

#include "ThePURGE.hpp"

namespace game {

struct UserStatistics {
    static auto draw(ThePURGE &game, entt::registry &world) -> void
    {
        static GLuint texture = engine::DrawableFactory::createtexture("data/textures/InfoHud.png");

        const auto infoHealth = world.get<Health>(game.player);
        const auto HP = infoHealth.current / infoHealth.max;
        const auto Atk = world.get<AttackDamage>(game.player);
        const auto level = world.get<Level>(game.player);
        const auto XP = static_cast<float>(level.current_xp) / static_cast<float>(level.xp_require);
        const auto keyPicker = world.get<KeyPicker>(game.player);

        ImGui::Begin(
            "Info Player",
            nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize
                | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
        ImVec2 size = ImGui::GetWindowSize();
        ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>((texture))), ImVec2(size.x - 30, size.y - 10));
        ImGui::SetCursorPos(ImVec2(ImGui::GetItemRectMin().x + 40, ImGui::GetItemRectMin().y + size.y / 7));
        ImGui::ProgressBar(HP, ImVec2(0.f, 0.f), fmt::format("{}/{}", infoHealth.current, infoHealth.max).data());
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("HP");
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
        ImGui::ProgressBar(XP, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("XP");
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
        helper::ImGui::Text("Level: {}", level.current_level);
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
        helper::ImGui::Text("Speed: {}", 1);
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
        helper::ImGui::Text("Atk: {}", Atk.damage);
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
        if (keyPicker.hasKey) helper::ImGui::Text("You have the key");
        ImGui::End();
    }
};

} // namespace game
