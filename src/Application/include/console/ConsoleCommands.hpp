#pragma once

#include <unordered_map>
#include <vector>
#include <functional>

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
    static void cmd_kill(entt::registry &world, ThePurge &game, std::vector<std::string> &&args);
    static void cmd_setSpell(entt::registry &world, ThePurge &game, std::vector<std::string> &&args);
    static void cmd_addXp(entt::registry &world, ThePurge &game, std::vector<std::string> &&args);
    static void cmd_addLevel(entt::registry &world, ThePurge &game, std::vector<std::string> &&args);
    static void cmd_setMusicVolume(entt::registry &world, ThePurge &game, std::vector<std::string> &&args);


private:
    std::unordered_map<std::string, handler_t> m_commands;
};


} // namespace game