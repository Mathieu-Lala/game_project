#pragma once

#include <cstring>

#include <vector>

#include <fmt/format.h>

#include "Engine/Graphics/third_party.hpp"

namespace engine {

namespace widget {

class ImGuiConsole {
public:
    using cmdHandler_t = std::function<void(const std::string_view)>;

    auto setCommandHandler(const cmdHandler_t &handler) -> void { m_cmdHandler = handler; }

    auto setCommands(std::vector<std::string> &&commands) -> void { m_commands = std::move(commands); }

    void logClear() { m_items.clear(); }

    template<typename... Args>
    auto logAdd(const std::string_view fmt, Args &&... args) -> void
    {
        m_items.push_back(fmt::format(fmt, std::forward<Args>(args)...));
    }

    void draw(bool *p_open = nullptr)
    {
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Console", p_open)) {
            ImGui::End();
            return;
        }

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Close Console")) *p_open = false;
            ImGui::EndPopup();
        }

        ImGui::TextWrapped("This example implements a console with basic coloring, completion (TAB key) and history "
                           "(Up/Down keys). A more elaborate "
                           "implementation may want to store entries along with extra data such as timestamp, emitter, "
                           "etc.");
        ImGui::TextWrapped("Enter 'HELP' for help.");

        if (ImGui::SmallButton("Add Debug Text")) {
            logAdd("{} some text", m_items.size());
            logAdd("some more text");
            logAdd("display very important message here!");
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Add Debug Error")) { logAdd("[error] something went wrong"); }
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear")) { logClear(); }
        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::SmallButton("Copy");
        ImGui::Separator();

        if (ImGui::BeginPopup("Options")) {
            ImGui::Checkbox("Auto-scroll", &m_autoscroll);
            ImGui::EndPopup();
        }

        if (ImGui::Button("Options")) ImGui::OpenPopup("Options");
        ImGui::SameLine();
        m_filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        ImGui::Separator();

        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild(
            "ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::Selectable("Clear")) logClear();
            ImGui::EndPopup();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
        if (copy_to_clipboard) ImGui::LogToClipboard();

        const auto get_color = [](const auto &item) -> std::optional<ImVec4> {
            if (item.find("[error]") != std::string::npos) {
                return ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
            } else if (item.find("[warn]") != std::string::npos) {
                return ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
            } else {
                return {};
            }
        };

        for (const auto &item : m_items) {
            if (!m_filter.PassFilter(item.data())) continue;
            const auto color = get_color(item);
            if (color.has_value()) ImGui::PushStyleColor(ImGuiCol_Text, color.value());
            ImGui::TextUnformatted(item.data());
            if (color.has_value()) ImGui::PopStyleColor();
        }
        if (copy_to_clipboard) ImGui::LogFinish();

        if (m_scroll_to_bottom || (m_autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        m_scroll_to_bottom = false;

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Separator();

        bool reclaim_focus = false;
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion
                                               | ImGuiInputTextFlags_CallbackHistory;
        if (ImGui::InputText(
                "Input",
                m_input_buffer.data(),
                m_input_buffer.size(),
                input_text_flags,
                [](ImGuiInputTextCallbackData *data) -> int {
                    return reinterpret_cast<ImGuiConsole *>(data->UserData)->TextEditCallback(data);
                },
                reinterpret_cast<void *>(this))) {
            auto s = m_input_buffer.data();
            Strtrim(s);
            if (s[0]) ExecCommand(s);
            strcpy(s, "");
            reclaim_focus = true;
        }

        ImGui::SetItemDefaultFocus();
        if (reclaim_focus) ImGui::SetKeyboardFocusHere(-1);

        ImGui::End();
    }

private:
    std::array<char, 256ul> m_input_buffer{0};
    std::vector<std::string> m_items;
    std::vector<std::string> m_commands;
    std::vector<std::string> m_history;
    int m_history_pos = -1;
    ImGuiTextFilter m_filter;
    bool m_autoscroll = true;
    bool m_scroll_to_bottom = false;

    cmdHandler_t m_cmdHandler;

    static void Strtrim(char *s)
    {
        char *str_end = s + strlen(s);
        while (str_end > s && str_end[-1] == ' ') str_end--;
        *str_end = 0;
    }

    void ExecCommand(const std::string_view command_line)
    {
        logAdd("# {}\n", command_line.data());

        m_history_pos = -1;
        m_history.erase(
            std::remove_if(
                m_history.begin(), m_history.end(), [&command_line](const auto &i) { return i == command_line; }),
            m_history.end());
        m_history.emplace_back(command_line);
        m_cmdHandler(command_line);
        m_scroll_to_bottom = true;
    }

    int TextEditCallback(ImGuiInputTextCallbackData *data)
    {
        switch (data->EventFlag) {
        case ImGuiInputTextFlags_CallbackCompletion: {
            const char *word_end = data->Buf + data->CursorPos;
            const char *word_start = word_end;
            while (word_start > data->Buf) {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';') break;
                word_start--;
            }

            const auto candidates = [&](const auto &db) {
                std::vector<std::string_view> out;
                std::copy_if(db.begin(), db.end(), std::back_inserter(out), [&](auto &command) {
                    return std::strncmp(command.data(), word_start, static_cast<std::size_t>(word_end - word_start)) == 0;
                });
                return out;
            }(m_commands);

            if (candidates.size() == 0) {
                logAdd("No match for \"{}\"!\n", word_start);
            } else if (candidates.size() == 1) {
                data->DeleteChars(static_cast<int>(word_start - data->Buf), static_cast<int>(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates.at(0).data());
                data->InsertChars(data->CursorPos, " ");
            } else {
                auto match_len = static_cast<std::size_t>(word_end - word_start);
                for (;;) {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (std::size_t i = 0; i < candidates.size() && all_candidates_matches; i++)
                        if (i == 0)
                            c = std::toupper(candidates[i][match_len]);
                        else if (c == 0 || c != std::toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    if (!all_candidates_matches) break;
                    match_len++;
                }

                if (match_len > 0ul) {
                    data->DeleteChars(static_cast<int>(word_start - data->Buf), static_cast<int>(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0].data(), &candidates[0].at(match_len));
                }

                logAdd("Possible matches:\n");
                for (auto i = 0ul; i < candidates.size(); i++) { logAdd("- {}\n", candidates[i]); }
            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory: {
            const auto prev_history_pos = m_history_pos;
            if (data->EventKey == ImGuiKey_UpArrow) {
                if (m_history_pos == -1)
                    m_history_pos = static_cast<int>(m_history.size() - 1);
                else if (m_history_pos > 0)
                    m_history_pos--;
            } else if (data->EventKey == ImGuiKey_DownArrow) {
                if (m_history_pos != -1)
                    if (++m_history_pos >= static_cast<int>(m_history.size())) m_history_pos = -1;
            }

            if (prev_history_pos != m_history_pos) {
                const auto history_str = (m_history_pos >= 0) ? m_history[static_cast<std::size_t>(m_history_pos)] : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str.data());
            }
        }
        }
        return 0;
    }
};

} // namespace widget

} // namespace engine
