#pragma once

#include <memory>

#include <Engine/Event/Event.hpp>
#include <Engine/api/Game.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/Camera.hpp>
#include <Engine/audio/Sound.hpp>

#include "component/all.hpp"

#include "widgets/debug/console/DebugConsole.hpp"

#include "menu/AMenu.hpp"

#include "level/MapGenerator.hpp"
#include "GameLogic.hpp"
#include "models/ClassDatabase.hpp"

namespace game {

class GameLogic;

class ThePURGE : public engine::api::Game {
public:
    ThePURGE();

    auto onCreate(entt::registry &world) -> void final;

    auto onUpdate(entt::registry &world, const engine::Event &e) -> void final;

    auto onDestroy(entt::registry &) -> void final;

    auto drawUserInterface(entt::registry &world) -> void final;

    void setMenu(std::unique_ptr<AMenu> &&menu)
    {
        m_currentMenu = std::move(menu);
    }


    // TODO DELETEME : useless, we never get to see the background color
    auto getBackgroundColor() const noexcept -> glm::vec3 final
    {
        return glm::vec3{0.45f, 0.55f, 0.60f};
    }

    auto getLogics() -> GameLogic & { return *m_logics; }
    auto getMusic() -> std::shared_ptr<engine::Sound> { return m_dungeonMusic; }

    auto getClassDatabase() -> const classes::Database & { return m_classDatabase; }
    auto getCamera() -> engine::Camera & { return m_camera; }

    auto logics() const noexcept -> const std::unique_ptr<GameLogic> & { return m_logics; }

public:
    entt::entity player; // note : should not require to keep it like that

private:
    std::unique_ptr<GameLogic> m_logics;

    classes::Database m_classDatabase;

    engine::Camera m_camera; // note : should be in engine::Core
    std::unique_ptr<DebugConsole> m_debugConsole;
    std::shared_ptr<engine::Sound> m_dungeonMusic;

    std::unique_ptr<AMenu> m_currentMenu;
};

} // namespace game
