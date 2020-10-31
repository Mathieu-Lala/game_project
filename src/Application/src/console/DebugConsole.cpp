#include "console/DebugConsole.hpp"
#include <spdlog/spdlog.h>
#include <sstream>

#include "Engine/Core.hpp"

game::DebugConsole::DebugConsole(ThePurge &game) : m_game(game)
{
    m_console.SetCommandHandler([this](auto &cmd) { handleCmd(cmd); });
    m_console.SetCommandAutoCompletion(m_commands.getCommands());
}

auto game::DebugConsole::info(const std::string &str) { m_console.AddLog(str.c_str()); }
auto game::DebugConsole::warn(const std::string &str) { m_console.AddLog("[warn] %s", str.c_str()); }
auto game::DebugConsole::error(const std::string &str) { m_console.AddLog("[error] %s", str.c_str()); }

void game::DebugConsole::handleCmd(const std::string &line)
{
    static engine::Core::Holder holder{};

    std::string cmd;
    std::vector<std::string> args;

    std::stringstream ss(line);

    ss >> cmd;

    std::string buff;
    while (ss >> buff) args.push_back(std::move(buff));

    if (cmd.empty()) return;

    CommandHandler::handler_t handler;

    try {
        handler = m_commands.getCommandHandler(cmd);
    } catch (const std::out_of_range &) {
        warn(fmt::format("Unknown command {}", cmd));
        return;
    }

    try {
        handler(holder.instance->getWorld(), m_game, std::move(args));
    } catch (const std::exception &e) {
        error(e.what());
    }
}
