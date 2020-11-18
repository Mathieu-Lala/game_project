#pragma once

#include <string_view>

#include <entt/entt.hpp>

#include <Engine/widgets/ImGuiConsole.hpp>

#include "ConsoleCommands.hpp"

namespace game {

class ThePURGE;

class DebugConsole {
public:
    explicit DebugConsole(ThePURGE &game);

    auto draw() -> void { m_console.draw(); }

    auto clear() -> void { m_console.logClear(); }

    template<typename... Args>
    void info(const std::string_view fmt, Args &&... args)
    {
        m_console.logAdd(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(const std::string_view fmt, Args &&... args)
    {
        m_console.logAdd(fmt::format("[warn] {}", fmt), std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(const std::string_view fmt, Args &&... args)
    {
        m_console.logAdd(fmt::format("[error] {}", fmt), std::forward<Args>(args)...);
    }

    auto handleCmd(const std::string_view cmd) -> void;

private:
    engine::widget::ImGuiConsole m_console;
    CommandHandler m_commands;

    ThePURGE &m_game;
};

} // namespace game
