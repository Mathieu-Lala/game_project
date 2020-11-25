#pragma once

#include <entt/entt.hpp>

#include "ThePURGE.hpp"

namespace game {

struct UserStatistics {
    static auto draw(ThePURGE &game, entt::registry &world) -> void
    {
        auto holder = engine::Core::Holder{};
        static GLuint texture =
            holder.instance->getCache<engine::Texture>()
                .load<engine::LoaderTexture>(
                    entt::hashed_string{fmt::format(
                                            "resource/texture/identifier/{}",
                                            holder.instance->settings().data_folder + "/textures/InfoHud.png")
                                            .data()},
                    holder.instance->settings().data_folder + "/textures/InfoHud.png")
                ->id;

        const auto infoHealth = world.get<Health>(game.player);
        const auto HP = infoHealth.current / infoHealth.max;
        // const auto Atk = world.get<AttackDamage>(game.player);
        const auto level = world.get<Level>(game.player);
        const auto XP = static_cast<float>(level.current_xp) / static_cast<float>(level.xp_require);

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
        helper::ImGui::Text("LEVEL {} ~~~ POINT {}", level.current_level, world.get<SkillPoint>(game.player).count);
        // ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
        // ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
        // helper::ImGui::Text("Speed: {}", 1);
        // ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
        // helper::ImGui::Text("Atk: {}", Atk.damage);

        for (const auto &i : world.get<SpellSlots>(game.player).spells) {
            if (i.has_value()) {
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
                ImGui::ProgressBar(
                    static_cast<float>(i.value().cd.remaining_cooldown.count())
                        / static_cast<float>(i.value().cd.cooldown.count()),
                    ImVec2(0.f, 0.f),
                    fmt::format("{}/{}", i.value().cd.remaining_cooldown.count(), i.value().cd.cooldown.count()).data());
            }
        }
        if (world.get<KeyPicker>(game.player).hasKey) {
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 49, ImGui::GetCursorPosY()));
            helper::ImGui::Text("You have the key");
        }

        ImGui::End();
    }
};

} // namespace game
