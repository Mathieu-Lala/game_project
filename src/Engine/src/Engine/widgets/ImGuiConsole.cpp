#include <cstring>
#include <string>

#include "Engine/widgets/ImGuiConsole.hpp"

static void Strtrim(char *s)
{
    auto str_end = s + strlen(s);
    while (str_end > s && str_end[-1] == ' ') str_end--;
    *str_end = 0;
}

// todo : cleaner
auto engine::widget::ImGuiConsole::trim(std::string_view in) -> std::optional<std::string>
{
    std::string s = in.data();
    Strtrim(s.data());
    if (s[0])
        return s;
    else
        return {};
}

void engine::widget::ImGuiConsole::draw(bool *p_open)
{
    ImGui::SetNextWindowSize(ImVec2(520, 300), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("console", p_open, ImGuiWindowFlags_NoDecoration)) { return ImGui::End(); }

    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Close Console")) *p_open = false;
        ImGui::EndPopup();
    }

    if (ImGui::SmallButton("Clear")) { logClear(); }
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");

    ImGui::Separator();

    if (ImGui::BeginPopup("Options")) {
        ImGui::Checkbox("Auto-scroll", &AutoScroll);
        ImGui::EndPopup();
    }

    if (ImGui::Button("Options")) ImGui::OpenPopup("Options");
    ImGui::SameLine();
    Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    ImGui::Separator();

    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::Selectable("Clear")) logClear();
        ImGui::EndPopup();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
    if (copy_to_clipboard) ImGui::LogToClipboard();
    for (const auto &item : m_items) {
        if (!Filter.PassFilter(item.data())) continue;

        const auto color = [&item]() -> std::optional<ImVec4> {
            if (item.find("[error]") != std::string::npos) {
                return ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
            } else if (item.find("[warn]") != std::string::npos) {
                return ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
            } else {
                return {};
            }
        }();

        if (color.has_value()) ImGui::PushStyleColor(ImGuiCol_Text, color.value());
        ImGui::TextUnformatted(item.data());
        if (color.has_value()) ImGui::PopStyleColor();
    }
    if (copy_to_clipboard) ImGui::LogFinish();

    if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) ImGui::SetScrollHereY(1.0f);
    ScrollToBottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    bool reclaim_focus = false;
    if (ImGui::InputText(
            "Input",
            m_input_buffer.data(),
            m_input_buffer.size(),
            ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory,
            [](ImGuiInputTextCallbackData *data) {
                return static_cast<ImGuiConsole *>(data->UserData)->callback_textEdit(data);
            },
            static_cast<void *>(this))) {
        execute_command(trim(m_input_buffer.data()).value_or("syntax error"));
        reclaim_focus = true;
    }

    ImGui::SetItemDefaultFocus();
    if (reclaim_focus) ImGui::SetKeyboardFocusHere(-1);

    ImGui::End();
}

void engine::widget::ImGuiConsole::execute_command(const std::string_view command_line)
{
    logAdd("# {}\n", command_line);

    m_history_pos = -1;
    for (auto i = static_cast<std::int64_t>(m_history.size()) - 1l; i >= 0l; i--)
        if (m_history.at(static_cast<std::size_t>(i)) == command_line) {
            m_history.erase(m_history.begin() + i);
            break;
        }
    m_history.push_back(std::string(command_line));

    m_cmdHandler(command_line);
    ScrollToBottom = true;
}

int engine::widget::ImGuiConsole::callback_textEdit(ImGuiInputTextCallbackData *data)
{
    switch (data->EventFlag) {
    case ImGuiInputTextFlags_CallbackCompletion: {
        auto word_end = data->Buf + data->CursorPos;
        auto word_start = word_end;
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
            logAdd("No match for \"{:.*s}\"!\n", static_cast<int>(word_end - word_start), word_start);
        } else if (candidates.size() == 1) {
            data->DeleteChars(static_cast<int>(word_start - data->Buf), static_cast<int>(word_end - word_start));
            data->InsertChars(data->CursorPos, candidates[0].data());
            data->InsertChars(data->CursorPos, " ");
        } else {
            auto match_len = static_cast<std::size_t>(word_end - word_start);
            for (;;) {
                auto c = 0;
                auto all_candidates_matches = true;
                for (auto i = 0ul; i < candidates.size() && all_candidates_matches; i++) {
                    if (i == 0ul) {
                        c = toupper(candidates[i][match_len]);
                    } else if (c == 0 || c != toupper(candidates[i][match_len])) {
                        all_candidates_matches = false;
                    }
                }
                if (!all_candidates_matches) break;
                match_len++;
            }

            if (match_len > 0) {
                data->DeleteChars(static_cast<int>(word_start - data->Buf), static_cast<int>(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0].data(), candidates[0].data() + match_len);
            }

            logAdd("Possible matches:\n");
            for (auto i = 0ul; i < candidates.size(); i++) logAdd("- {}\n", candidates[i]);
        }

        break;
    }
    case ImGuiInputTextFlags_CallbackHistory: {
        const int prev_history_pos = m_history_pos;
        if (data->EventKey == ImGuiKey_UpArrow) {
            if (m_history_pos == -1) {
                m_history_pos = static_cast<int>(m_history.size() - 1);
            } else if (m_history_pos > 0) {
                m_history_pos--;
            }
        } else if (data->EventKey == ImGuiKey_DownArrow) {
            if (m_history_pos != -1) {
                if (++m_history_pos >= static_cast<int>(m_history.size())) { m_history_pos = -1; }
            }

            if (prev_history_pos != m_history_pos) {
                const auto history_str = m_history_pos >= 0 ? m_history.at(static_cast<std::size_t>(m_history_pos)) : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str.data());
            }
        }
    }
    }
    return 0;
}
