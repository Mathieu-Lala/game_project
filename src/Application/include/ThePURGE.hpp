#pragma once

#include <memory>

#include <Engine/Event/Event.hpp>
#include <Engine/api/Game.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/Camera.hpp>
#include <Engine/audio/Sound.hpp>

#include "component/all.hpp"

#include "menu/AMenu.hpp"

#include "level/MapGenerator.hpp"
#include "GameLogic.hpp"
#include "models/Class.hpp"

#ifndef NDEBUG
# include "widgets/debug/console/DebugConsole.hpp"
#endif

namespace game {

class GameLogic;

class ThePURGE : public engine::api::Game {
public: // api
    ThePURGE() = default;

    auto onCreate(entt::registry &world) -> void final;

    auto onUpdate(entt::registry &world, const engine::Event &e) -> void final;

    auto onDestroy(entt::registry &) -> void final;

    auto drawUserInterface(entt::registry &world) -> void final;

    auto getBackgroundColor() const noexcept -> glm::vec3 final { return {0.0f, 0.0f, 0.0f}; }

public:
    void setMenu(std::unique_ptr<AMenu> &&menu) { m_currentMenu = std::move(menu); }

    auto getLogics() -> GameLogic & { return *m_logics; }
    auto getBackgroundMusic() -> std::shared_ptr<engine::Sound> { return m_background_music; }

    auto getClassDatabase() { return m_class_db; }
    auto getCamera() -> engine::Camera & { return m_camera; }

    auto logics() const noexcept -> const std::unique_ptr<GameLogic> & { return m_logics; }

    entt::entity player; // note : remove me

private:

#ifndef NDEBUG
    std::unique_ptr<DebugConsole> m_console;
#endif

    std::unique_ptr<GameLogic> m_logics;

    ClassDatabase m_class_db;

    engine::Camera m_camera; // note : should be in engine::Core
    std::shared_ptr<engine::Sound> m_background_music;

    std::unique_ptr<AMenu> m_currentMenu{ nullptr };
};

} // namespace game
