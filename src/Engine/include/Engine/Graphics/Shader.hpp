#pragma once

#include <string_view>
#include <string>

#include <glm/matrix.hpp>

namespace engine {

class Shader {
public:
    Shader() = default;

    Shader(const std::string_view vCode, const std::string_view fCode);

    // note : could use this https://github.com/joboccara/NamedType
    // but there is no conan package :(

    static auto fromFile(const std::string_view vFile, const std::string_view fFile) -> Shader;

    ~Shader();

    auto use() -> void;

    template<typename T>
    auto setUniform(const std::string_view, T) -> void;

private:
    std::uint32_t ID;
};

template<>
auto Shader::setUniform(const std::string_view, bool) -> void;

template<>
auto Shader::setUniform(const std::string_view, float) -> void;

template<>
auto Shader::setUniform(const std::string_view, glm::mat4) -> void;

} // namespace engine
