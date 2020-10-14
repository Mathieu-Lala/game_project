#pragma once

#include <Engine/Event/Event.hpp>
#include <Engine/Game.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/Camera.hpp>

#include "component/all.hpp"

#include "Declaration.hpp" // note : for DATA_DIR

namespace game {

class ThePurge : public engine::Game {
    // todo : move shaders logics in Core
    engine::Shader shader =
        engine::Shader::fromFile(DATA_DIR "/shaders/camera.vert.glsl", DATA_DIR "/shaders/camera.frag.glsl");

    entt::entity player;

    // Init movement signal player
    entt::sigh<void(entt::registry &, entt::entity &, const engine::d2::Acceleration &)> movement;
    entt::sink<void(entt::registry &, entt::entity &, const engine::d2::Acceleration &)> sinkMovement{movement};

    // entityLogic signal loop
    entt::sigh<void(entt::registry &, entt::entity &, const engine::TimeElapsed &)> gameLogic;
    entt::sink<void(entt::registry &, entt::entity &, const engine::TimeElapsed &)> sinkGameLogic{gameLogic};

    entt::sigh<void(void)> castSpell;
    entt::sink<void(void)> sinkCastSpell{castSpell};

    auto onCreate(entt::registry &world) -> void final;

    auto onUpdate(entt::registry &world, const engine::Event &e) -> void final;

    auto mapGenerationOverlayTick(entt::registry &world) -> void;

    auto onDestroy(entt::registry &) -> void final;

    auto drawUserInterface(entt::registry &world) -> void final;

private:
    engine::Camera m_camera;
};

} // namespace game
