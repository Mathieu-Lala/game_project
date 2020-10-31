#include "console/consoleCommands.hpp"
#include <spdlog/spdlog.h>
#include "console/ArgParsingUtils.hpp"

#include "ThePURGE.hpp"

game::CommandHandler::CommandHandler() { m_commands["kill"] = cmd_kill; }

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

void game::CommandHandler::cmd_kill(entt::registry &world, ThePurge &game, std::vector<std::string> &&args)
{
    std::string what;

    try {
        if (args.size() != 1) throw std::runtime_error("Wrong argument count");

        what = lexicalCast<std::string>(args[0]);

        if (what == "player") {
            for (auto &e : world.view<entt::tag<"player"_hs>>()) game.getLogics().entity_killed(world, e, e);
        } else if (what == "boss") {
            for (auto &e : world.view<entt::tag<"boss"_hs>>()) game.getLogics().entity_killed(world, e, game.player);

        } else
            throw std::runtime_error(fmt::format("Invalid argument {}", what));

    } catch (const std::runtime_error &e) {
        throw std::runtime_error(fmt::format("{}\nusage: kill boss|player", e.what()));
    }
}
