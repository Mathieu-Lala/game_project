#pragma once

#include <memory>

#include <Engine/Event/Event.hpp>
#include <Engine/api/Game.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/Camera.hpp>
#include <Engine/audio/Sound.hpp>

#include "component/all.hpp"

#include "console/DebugConsole.hpp"

#include "level/MapGenerator.hpp"
#include "GameLogic.hpp"
#include "models/ClassDatabase.hpp"

namespace game {

class GameLogic;

class ThePURGE : public engine::api::Game {
public:
    ThePURGE();

    enum class State {
        LOADING,
        IN_GAME,
        IN_INVENTORY,
        GAME_OVER,
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
        return m_state == State::GAME_OVER ? glm::vec3{0.35f, 0.45f, 0.50f} : glm::vec3{0.45f, 0.55f, 0.60f};
    }

    auto getLogics() -> GameLogic & { return *m_logics; }
    auto getMusic() -> std::shared_ptr<engine::Sound> { return m_dungeonMusic; }

    auto getClassDatabase() -> const classes::Database & { return m_classDatabase; }
    auto getCamera() -> engine::Camera & { return m_camera; }

    auto logics() const noexcept -> const std::unique_ptr<GameLogic> & { return m_logics; }

private:
    // auto goToNextFloor(entt::registry &world) -> void;

    auto mapGenerationOverlayTick(entt::registry &world) -> void;
    auto displaySoundDebugGui() -> void;

    State m_state{State::LOADING};

    FloorGenParam m_map_generation_params;
    std::uint32_t m_nextFloorSeed;

    engine::Camera m_camera; // note : should be in engine::Core

    std::unique_ptr<GameLogic> m_logics;

    std::shared_ptr<engine::Sound> m_dungeonMusic;

    std::unique_ptr<DebugConsole> m_debugConsole;

    classes::Database m_classDatabase;
};

} // namespace game
