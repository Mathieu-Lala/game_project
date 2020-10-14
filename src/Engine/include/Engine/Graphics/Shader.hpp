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

    auto uploadUniformMat4(const std::string &name, const glm::mat4 &mat) -> void;

private:
    std::uint32_t ID;

    // std::string debug_vertexCode;
};

} // namespace engine
