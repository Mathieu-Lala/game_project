#pragma once

#include <string_view>
#include <string>

#include <glm/mat4x4.hpp>

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

    // note : this could be templated
    auto uploadUniformMat4(const std::string &name, const glm::mat4 &mat) -> void;

    template<typename T>
    auto setUniform(const std::string_view, std::decay_t<T>) -> void;

private:
    std::uint32_t ID;

    // std::string debug_vertexCode;
};

template<>
auto Shader::setUniform<bool>(const std::string_view, bool) -> void;

template<>
auto Shader::setUniform<float>(const std::string_view, float) -> void;

} // namespace engine
