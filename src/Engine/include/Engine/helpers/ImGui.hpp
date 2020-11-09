#pragma once

#include <imgui.h>
#include <string_view>
#include <fmt/format.h>

namespace helper {

namespace ImGui {

template<typename... Param>
inline void Text(std::string_view format, Param &&... param)
{
    ::ImGui::TextUnformatted(fmt::format(format, std::forward<Param>(param)...).c_str());
}

inline bool Button(std::string id, ImVec4 selectedColor) { 
    bool pressed;
    ::ImGui::PushStyleColor(ImGuiCol_Button, selectedColor);
    pressed =::ImGui::Button(id.c_str());
    ::ImGui::PopStyleColor(1);
    return pressed;
}

} // namespace ImGui

} // namespace helper
