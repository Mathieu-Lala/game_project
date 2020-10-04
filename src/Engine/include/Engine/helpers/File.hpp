#pragma once

#include <fstream>
#include <optional>
#include <string>
#include <string_view>

namespace engine {

auto getFileContent(const std::string_view file) -> std::optional<std::string>
{
    std::ifstream t(file.data());
    if (!t) return {};
    return std::string(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>());
}

} // namespace engine
