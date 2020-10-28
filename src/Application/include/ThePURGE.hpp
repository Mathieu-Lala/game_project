#pragma once

#include <Engine/Event/Event.hpp>
#include <Engine/Game.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/Camera.hpp>

#include "component/all.hpp"

namespace game {

class GameLogic;

class ThePurge : public engine::Game {
public:
    ThePurge();

    enum State {
        LOADING,
        IN_GAME,
        GAME_OVER
    };

    auto onCreate(entt::registry &world) -> void final;

    auto onUpdate(entt::registry &world, const engine::Event &e) -> void final;

    auto onDestroy(entt::registry &) -> void final;

    auto drawUserInterface(entt::registry &world) -> void final;

    entt::entity player; // note : should not require to keep it like that

    auto setState(State new_state) noexcept { m_state = new_state; }

    // constexpr // note : C++20 but not supported by MSVC yet
    auto getBackgroundColor() const noexcept -> glm::vec3 final
    {
        return m_state == GAME_OVER ? glm::vec3{0.35f, 0.45f, 0.50f} : glm::vec3{0.45f, 0.55f, 0.60f};
    }

private:
    auto goToNextFloor(entt::registry &world) -> void;

    auto mapGenerationOverlayTick(entt::registry &world) -> void;
    auto displaySoundDebugGui() -> void;

    State m_state{LOADING};

    FloorGenParam m_map_generation_params;
    std::uint32_t m_nextFloorSeed;

    engine::Camera m_camera; // note : should be in engine::Core

    GameLogic m_logics;

    std::shared_ptr<engine::Sound> m_dungeonMusic;
};

} // namespace game
