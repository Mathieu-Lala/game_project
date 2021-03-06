#include "widgets/debug/console/DebugConsole.hpp"
#include <spdlog/spdlog.h>
#include <sstream>

#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/Core.hpp>

game::DebugConsole::DebugConsole(ThePURGE &game) : m_game(game)
{
    m_console.setCommandHandler([this](const std::string_view cmd) { handleCmd(cmd); });
    m_console.setCommands(m_commands.getCommands());
}

void game::DebugConsole::handleCmd(const std::string_view line)
{
    static auto holder = engine::Core::Holder{};

    std::string cmd;
    std::vector<std::string> args;

    std::stringstream ss(line.data());

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
        handler(holder.instance->getWorld(), m_game, std::move(args), *this);
    } catch (const std::exception &e) {
        error(e.what());
    }
}
