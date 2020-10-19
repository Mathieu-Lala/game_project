#pragma once

#include <Engine/Event/Event.hpp>
#include <Engine/Game.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/Camera.hpp>
#include <Engine/audio/AudioManager.hpp>

#include "component/all.hpp"

#include "Declaration.hpp" // note : for DATA_DIR

#include "level/MapGenerator.hpp"

namespace game {

class GameLogic;

class ThePurge : public engine::Game {
public:
    ThePurge();

    enum State {
        LOADING, // pre-game
        IN_GAME,
        GAME_OVER
    };

    auto onCreate(entt::registry &world) -> void final;

    auto onUpdate(entt::registry &world, const engine::Event &e) -> void final;

    auto onDestroy(entt::registry &) -> void final;

    auto drawUserInterface(entt::registry &world) -> void final;

    entt::entity player;

    // todo : move shaders logics in Core
    engine::Shader shader =
        engine::Shader::fromFile(DATA_DIR "/shaders/camera.vert.glsl", DATA_DIR "/shaders/camera.frag.glsl");

    auto setState(State new_state) noexcept { m_state = new_state; }

private:

    auto goToNextFloor(entt::registry &world) -> void;

    auto mapGenerationOverlayTick(entt::registry &world) -> void;
    auto displaySoundDebugGui() -> void;

    State m_state{ LOADING };

    FloorGenParam m_map_generation_params;
    unsigned int m_nextFloorSeed;

    engine::Camera m_camera;
    engine::AudioManager m_audioManager;

    GameLogic m_logics;
};

} // namespace game
