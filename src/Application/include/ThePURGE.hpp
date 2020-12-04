#pragma once

#include <memory>

#include <Engine/Event/Event.hpp>
#include <Engine/api/Game.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/Camera.hpp>
#include <Engine/audio/Sound.hpp>

#include "menu/AMenu.hpp"

#include "GameLogic.hpp"
#include "models/Class.hpp"
#include "models/Spell.hpp"
#include "models/Enemy.hpp"
#include "models/Effect.hpp"

#ifndef NDEBUG
# include "widgets/debug/console/DebugConsole.hpp"
#endif

namespace game {

class GameLogic;

struct SpellDatabase;
struct ClassDatabase;
struct EnemyDatabase;

class ThePURGE : public engine::api::Game {
public: // api
    ThePURGE() = default;

    auto onCreate(entt::registry &world) -> void final;

    auto onUpdate(entt::registry &world, const engine::Event &e) -> void final;

    auto onDestroy(entt::registry &) -> void final;

    auto drawUserInterface(entt::registry &world) -> void final;

    auto getBackgroundColor() const noexcept -> glm::vec3 final { return {0.0f, 0.0f, 0.0f}; }

public:
    auto logics() const noexcept -> const std::unique_ptr<GameLogic> & { return m_logics; }

    auto dbSpells() noexcept -> SpellDatabase & { return m_db_spell; }
    auto dbClasses() noexcept -> ClassDatabase & { return m_db_class; }
    auto dbEnemies() noexcept -> EnemyDatabase & { return m_db_enemy; }
    auto dbEffects() noexcept -> EffectDatabase & { return m_db_effects; }

    auto getCamera() -> engine::Camera & { return m_camera; }
    void setMenu(std::unique_ptr<AMenu> &&menu) { m_currentMenu = std::move(menu);}

    auto getBackgroundMusic() -> std::shared_ptr<engine::Sound> { return m_background_music; }
    void setBackgroundMusic(const std::string &path, float volume = 1) noexcept;

    entt::entity player; // note : remove me

private:

#ifndef NDEBUG
    std::unique_ptr<DebugConsole> m_console;
#endif

    std::unique_ptr<GameLogic> m_logics;

    SpellDatabase m_db_spell;
    ClassDatabase m_db_class;
    EnemyDatabase m_db_enemy;
    EffectDatabase m_db_effects;

    engine::Camera m_camera; // note : should be in engine::Core
    std::shared_ptr<engine::Sound> m_background_music;

    std::unique_ptr<AMenu> m_currentMenu{ nullptr };
};

} // namespace game
