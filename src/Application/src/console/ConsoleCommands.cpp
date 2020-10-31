#include "console/consoleCommands.hpp"
#include <spdlog/spdlog.h>

game::CommandHandler::CommandHandler() { m_commands["test"] = cmd_test; }

auto game::CommandHandler::getCommandHandler(const std::string &cmd) -> const game::CommandHandler::handler_t &
{
    return m_commands.at(cmd);
}

void game::CommandHandler::cmd_test(std::vector<std::string> &&args)
{
    spdlog::info("Called test cmd with : ");
    for (auto &str : args) spdlog::info("\t{}", str);
}
