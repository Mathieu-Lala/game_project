#pragma once

#include <optional>
#include <string>

#include <magic_enum.hpp>

namespace game {

template<typename T>
static auto toEnum(std::string in) noexcept -> std::optional<T>
{
    const auto to_lower = [](auto str) {
        std::transform(str.begin(), str.end(), str.begin(), [](auto c) { return static_cast<char>(std::tolower(c)); });
        return str;
    };

    in = to_lower(in);

    for (const auto &i : magic_enum::enum_values<T>()) {
        if (in == to_lower(std::string{magic_enum::enum_name(i)})) { return static_cast<T>(i); }
    }
    return {};
}

} // namespace game
