#include "console/consoleCommands.hpp"
#include <spdlog/spdlog.h>

game::CommandHandler::CommandHandler() { m_commands["test"] = cmd_test; }

auto game::CommandHandler::getCommandHandler(const std::string &cmd) -> const game::CommandHandler::handler_t &
{
    return m_commands.at(cmd);
}

std::vector<std::string> game::CommandHandler::getCommands() const
{
    std::vector<std::string> result;

    result.reserve(m_commands.size());

    for (const auto &[k, v] : m_commands) result.push_back(k);

    return result;
}

void game::CommandHandler::cmd_test(std::vector<std::string> &&args)
{
    spdlog::info("Called test cmd with : ");
    for (auto &str : args) spdlog::info("\t{}", str);
}
