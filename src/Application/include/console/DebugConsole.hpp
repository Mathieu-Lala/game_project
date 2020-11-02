#pragma once

#include <string_view>

#include <entt/entt.hpp>

#include <Engine/widgets/ImGuiConsole.hpp>

#include "ConsoleCommands.hpp"

namespace game {

class ThePurge;

class DebugConsole {
public:
    explicit DebugConsole(ThePurge &game);

    auto draw() -> void { m_console.draw(); }

    auto clear() -> void { m_console.logClear(); }

    auto info(const std::string_view str) -> void;
    auto warn(const std::string_view str) -> void;
    auto error(const std::string_view str) -> void;

    auto handleCmd(const std::string_view cmd) -> void;

private:
    engine::widget::ImGuiConsole m_console;
    CommandHandler m_commands;

    ThePurge &m_game;
};

} // namespace game
