#include <spdlog/spdlog.h>
#include "Engine/Graphics/third_party.hpp"

#include "Engine/helpers/Warnings.hpp"

DISABLE_WARNING_PUSH
DISABLE_WARNING_NON_STD_EXTENSION
#include <glm/gtc/type_ptr.hpp>
DISABLE_WARNING_POP

#include "Engine/Graphics/Shader.hpp"
#include "Engine/helpers/File.hpp"

template<std::size_t type>
struct shader_ {
    explicit shader_(const char *source) : ID{::glCreateShader(type)}
    {
        ::glShaderSource(ID, 1, &source, nullptr);
        ::glCompileShader(ID);

        GLint success = 0;
        ::glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE) {
            GLint maxLength = 0;
            ::glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> errorLog(static_cast<std::size_t>(maxLength));
            ::glGetShaderInfoLog(ID, maxLength, &maxLength, errorLog.data());

            spdlog::error("(Failed to compile shader, \nError : {}\n\n{}\n)", errorLog.data(), source);
        }
    }

    ~shader_() { ::glDeleteShader(ID); }

    std::uint32_t ID;
};

engine::Shader::Shader(const std::string_view vCode, const std::string_view fCode) : ID{::glCreateProgram()}
{
    shader_<GL_VERTEX_SHADER> vertex{vCode.data()};
    shader_<GL_FRAGMENT_SHADER> fragment{fCode.data()};

    ::glAttachShader(ID, vertex.ID);
    ::glAttachShader(ID, fragment.ID);
    ::glLinkProgram(ID);

    GLint success;
    ::glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (const auto err = glGetError(); err != GL_NO_ERROR) {
        spdlog::error("Error {} : {}", err, glewGetErrorString(err));
        return;
    }

    if (success == GL_FALSE) {
        GLint maxLength = 0;
        ::glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(static_cast<std::size_t>(maxLength));
        ::glGetProgramInfoLog(ID, maxLength, &maxLength, errorLog.data());

        spdlog::error("(Failed to link shader program {}, \nError : {}\n", ID, errorLog.data());
    } else {
        spdlog::info("Successfully created shader program {}", ID);
    }
}

engine::Shader::~Shader()
{
    ::glDeleteProgram(ID);
    spdlog::info("Destroyed shader program {}", ID);
}

auto engine::Shader::fromFile(const std::string_view vFile, const std::string_view fFile) -> Shader
{
    return {
        getFileContent(vFile).value_or(fmt::format("Cannot open file: {}", vFile)),
        getFileContent(fFile).value_or(fmt::format("Cannot open file: {}", fFile))};
}

auto engine::Shader::use() -> void { ::glUseProgram(ID); }

template<>
auto engine::Shader::setUniform<bool>(const std::string_view name, bool v) -> void
{
    if (const auto location = glGetUniformLocation(ID, name.data()); location != -1)
        ::glUniform1ui(location, v);
}

template<>
auto engine::Shader::setUniform<float>(const std::string_view name, float v) -> void
{
    if (const auto location = glGetUniformLocation(ID, name.data()); location != -1)
        ::glUniform1f(location, v);
}

auto engine::Shader::uploadUniformMat4(const std::string &name, const ::glm::mat4 &mat) -> void
{
    use();

    if (const auto location = glGetUniformLocation(ID, name.c_str()); location != -1)
        ::glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    else {
        spdlog::warn("Could not find uniform '{}' in ID : {}", name, ID);

        GLint count;
        ::glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &count);

        if (auto err = glGetError(); err != GL_NO_ERROR) {
            spdlog::error("Error {} : {}", err, ::glewGetErrorString(err));
            return;
        }
        spdlog::warn("Active Uniforms: {}", count);

        GLsizei length;
        std::array<char, 16ul> gl_name;
        for (int i = 0; i < count; i++) {
            GLint size;
            GLenum type;
            ::glGetActiveUniform(
                ID, static_cast<GLuint>(i), static_cast<GLsizei>(gl_name.size()), &length, &size, &type, gl_name.data());
            spdlog::warn("  Uniform {} Type: {} Name: {}", i, type, gl_name.data());
        }
    }
}
