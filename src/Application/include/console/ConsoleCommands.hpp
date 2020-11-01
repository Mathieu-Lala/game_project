#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <entt/entt.hpp>

namespace game {

class ThePurge;

class CommandHandler {
public:
    using handler_t = std::function<void(entt::registry &world, ThePurge &, std::vector<std::string> &&args)>;

    CommandHandler();

    auto getCommandHandler(const std::string &cmd) -> const handler_t &;

    std::vector<std::string> getCommands() const;

private:
    static handler_t cmd_kill;
    static handler_t cmd_setSpell;
    static handler_t cmd_addXp;
    static handler_t cmd_addLevel;
    static handler_t cmd_setMusicVolume;

private:
    const std::unordered_map<std::string, handler_t> m_commands;
};

} // namespace game
