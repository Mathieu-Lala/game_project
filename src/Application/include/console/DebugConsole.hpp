#pragma once

#include <entt/entt.hpp>

#include "ImGuiConsole.hpp"
#include "ConsoleCommands.hpp"


namespace game {

class ThePurge;

class DebugConsole {
public:
    explicit DebugConsole(ThePurge &game);

    void draw() { m_console.Draw(); }

    void clear() { m_console.ClearLog(); }

    auto info(const std::string &str);
    auto warn(const std::string &str);
    auto error(const std::string &str);

    void handleCmd(const std::string &cmd);
private:
    ImGuiConsole m_console;
    CommandHandler m_commands;

    ThePurge &m_game;
};

} // namespace game
