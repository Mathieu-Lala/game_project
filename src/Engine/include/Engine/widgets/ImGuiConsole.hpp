#pragma once

#include <cctype>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include <array>
#include <functional>
#include <optional>
#include <vector>

#include <imgui.h>
#include <fmt/format.h>

// Taken from https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp

namespace engine {

namespace widget {

class ImGuiConsole {
private:
    using cmdHandler_t = std::function<void(const std::string_view)>;

public:
    auto logClear() -> void { m_items.clear(); }

    template<typename... Args>
    auto logAdd(const std::string_view fmt, Args &&... args) -> void
    {
        m_items.push_back(fmt::format(fmt, std::forward<Args>(args)...));
    }

    auto draw(bool *p_open = nullptr) -> void;

    auto setCommandHandler(const cmdHandler_t &handler) -> void { m_cmdHandler = handler; }

    auto setCommandAutoCompletion(std::vector<std::string> &&commands) -> void { m_commands = std::move(commands); }

private:
    auto execute_command(const std::string_view command_line) -> void;

    auto callback_textEdit(ImGuiInputTextCallbackData *data) -> int;

    auto trim(std::string_view) -> std::optional<std::string>;

private:
    std::array<char, 256ul> m_input_buffer{0};
    std::vector<std::string> m_items;
    std::vector<std::string> m_commands;
    std::vector<std::string> m_history;

    int m_history_pos = -1; // -1: new line, 0..History.Size-1 browsing history.

    ImGuiTextFilter Filter;
    bool AutoScroll = true;
    bool ScrollToBottom = false;
    cmdHandler_t m_cmdHandler;
};

} // namespace widget

} // namespace engine
