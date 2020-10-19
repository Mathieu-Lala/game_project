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

} // namespace ImGui

} // namespace helper
