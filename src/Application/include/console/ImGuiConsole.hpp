#pragma once

#include "imgui.h"

#include <ctype.h>  // toupper
#include <limits.h> // INT_MIN, INT_MAX
#include <math.h>   // sqrtf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>  // vsnprintf, sscanf, printf
#include <stdlib.h> // NULL, malloc, free, atoi
#include <stdint.h> // intptr_t

#include <functional>

// Taken from https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp

class ImGuiConsole {
    using cmdHandler_t = std::function<void(const std::string &)>;
public:
    ImGuiConsole();
    ~ImGuiConsole();

    void ClearLog();

    void AddLog(const char *fmt, ...) IM_FMTARGS(2);

    void Draw(bool *p_open = nullptr);

    void SetCommandHandler(const cmdHandler_t &handler) { m_cmdHandler = handler; }

    void SetCommandAutoCompletion(const std::vector<std::string> &commands);
private:
    void ExecCommand(const char *command_line);

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(ImGuiInputTextCallbackData *data)
    {
        ImGuiConsole *console = (ImGuiConsole *) data->UserData;
        return console->TextEditCallback(data);
    }

    int TextEditCallback(ImGuiInputTextCallbackData *data);

private:
    char InputBuf[256];
    ImVector<char *> Items;
    ImVector<const char *> Commands;
    ImVector<char *> History;
    int HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter Filter;
    bool AutoScroll;
    bool ScrollToBottom;
    cmdHandler_t m_cmdHandler;
};
