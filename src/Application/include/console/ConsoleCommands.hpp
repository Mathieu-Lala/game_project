#pragma once

#include <unordered_map>
#include <vector>
#include <functional>

namespace game {

class CommandHandler {
public:
    using handler_t = std::function<void(std::vector<std::string> &&args)>;
    
    CommandHandler();

    auto getCommandHandler(const std::string &cmd) -> const handler_t &;

    std::vector<std::string> getCommands() const;
private:
    static void cmd_test(std::vector<std::string> &&args);

private:
    std::unordered_map<std::string, handler_t > m_commands;
};


} // namespace game