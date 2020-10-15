#pragma once

#include <Engine/Event/Event.hpp>
#include <Engine/Game.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/Camera.hpp>

#include "component/all.hpp"

#include "Declaration.hpp" // note : for DATA_DIR

#include "level/MapGenerator.hpp"

namespace game {

class GameLogic;

class ThePurge : public engine::Game {
public:
    ThePurge();

    auto onCreate(entt::registry &world) -> void final;

    auto onUpdate(entt::registry &world, const engine::Event &e) -> void final;

    auto onDestroy(entt::registry &) -> void final;

    auto drawUserInterface(entt::registry &world) -> void final;

    entt::entity player;

    // todo : move shaders logics in Core
    engine::Shader shader =
        engine::Shader::fromFile(DATA_DIR "/shaders/camera.vert.glsl", DATA_DIR "/shaders/camera.frag.glsl");

private:

    auto mapGenerationOverlayTick(entt::registry &world) -> void;

    FloorGenParam m_map_generation_params;

    engine::Camera m_camera;

    GameLogic m_logics;
};

} // namespace game
