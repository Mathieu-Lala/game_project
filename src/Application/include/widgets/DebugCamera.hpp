#pragma once

#include <entt/entt.hpp>

#include "ThePURGE.hpp"

namespace game {

namespace widget {

struct DebugCamera {
    static auto draw(engine::Camera &cam)
    {
        ImGui::Begin("Camera");
        if (ImGui::Button("Reset")) cam = engine::Camera();

        auto cameraPos = cam.getCenter();
        helper::ImGui::Text("Camera Position ({}, {})", cameraPos.x, cameraPos.y);

        bool pos_updated = false;
        pos_updated |= ImGui::DragFloat("Camera X", &cameraPos.x);
        pos_updated |= ImGui::DragFloat("Camera Y", &cameraPos.y);

        if (pos_updated) cam.setCenter(cameraPos);

        auto viewPortSize = cam.getViewportSize();
        const auto pos = cam.getCenter();

        helper::ImGui::Text("Viewport size ({}, {})", viewPortSize.x, viewPortSize.y);
        ImGui::Text("Viewport range :");
        helper::ImGui::Text("   left  : {}", pos.x - viewPortSize.x / 2.0f);
        helper::ImGui::Text("   right : {}", pos.x + viewPortSize.x / 2.0f);
        helper::ImGui::Text("   top   : {}", pos.y + viewPortSize.y / 2.0f);
        helper::ImGui::Text("   bottom: {}", pos.y - viewPortSize.y / 2.0f);

        bool updated = false;
        updated |= ImGui::DragFloat("Viewport width", &viewPortSize.x, 1.f, 2.f);
        updated |= ImGui::DragFloat("Viewport height", &viewPortSize.y, 1.f, 2.f);

        if (updated) cam.setViewportSize(viewPortSize);
        ImGui::End();
    }
};

} // namespace widget

} // namespace game
