#pragma once

#include <entt/entt.hpp>

#include "ThePURGE.hpp"

namespace game {

namespace widget {

struct DebugAudio {
    static auto draw()
    {
        static auto holder = engine::Core::Holder{};

        static std::vector<std::shared_ptr<engine::Sound>> sounds;

        ImGui::Begin("Sound debug window");

        if (ImGui::Button("Load Music")) {
            try {
                sounds.push_back(holder.instance->getAudioManager().getSound(
                    holder.instance->settings().data_folder + "/sounds/dungeon_music.wav"));
            } catch (...) {
            }
        }
        if (ImGui::Button("Load Hit sound")) {
            try {
                sounds.push_back(holder.instance->getAudioManager().getSound(
                    holder.instance->settings().data_folder + "/sounds/hit.wav"));
            } catch (...) {
            }
        }
        ImGui::Separator();

        std::shared_ptr<engine::Sound> toRemove = nullptr;

        int loopId = 0;
        for (const auto &s : sounds) {
            ImGui::PushID(loopId++);

            ImGui::Text("Status :");
            ImGui::SameLine();

            switch (s->getStatus()) {
            case engine::SoundStatus::INITIAL: ImGui::TextColored(ImVec4(1, 1, 1, 1), "Initial"); break;
            case engine::SoundStatus::PLAYING: ImGui::TextColored(ImVec4(0.2f, 1, 0.2f, 1), "Playing"); break;
            case engine::SoundStatus::PAUSED: ImGui::TextColored(ImVec4(1, 1, 0.4f, 1), "Paused"); break;
            case engine::SoundStatus::STOPPED: ImGui::TextColored(ImVec4(1, 0.2f, 0.2f, 1), "Stopped"); break;
            }

            if (ImGui::Button("Play")) s->play();
            if (ImGui::Button("Sop")) s->stop();

            auto speed = s->getSpeed();
            if (ImGui::SliderFloat("Speed", &speed, 0.5, 2)) s->setSpeed(speed);

            auto volume = s->getVolume();
            if (ImGui::SliderFloat("Volume", &volume, 0, 5)) s->setVolume(volume);

            auto loop = s->doesLoop();
            if (ImGui::Checkbox("Loop", &loop)) s->setLoop(loop);


            if (ImGui::Button("Forget")) {
                toRemove = s;
                s->stop();
            }

            ImGui::PopID();

            ImGui::Separator();
        }

        if (toRemove) sounds.erase(std::find(std::begin(sounds), std::end(sounds), toRemove));

        ImGui::End();
    }
};

} // namespace widget

} // namespace game
