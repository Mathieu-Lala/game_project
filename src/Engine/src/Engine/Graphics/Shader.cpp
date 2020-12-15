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
        CALL_OPEN_GL(::glShaderSource(ID, 1, &source, nullptr));
        CALL_OPEN_GL(::glCompileShader(ID));

        GLint success = 0;
        CALL_OPEN_GL(::glGetShaderiv(ID, GL_COMPILE_STATUS, &success));
        if (success == GL_FALSE) {
            GLint maxLength = 0;
            CALL_OPEN_GL(::glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &maxLength));

            std::vector<GLchar> errorLog(static_cast<std::size_t>(maxLength));
            CALL_OPEN_GL(::glGetShaderInfoLog(ID, maxLength, &maxLength, errorLog.data()));

            spdlog::error("(Failed to compile shader, \nError : {}\n\n{}\n)", errorLog.data(), source);
        }
    }

    ~shader_() { CALL_OPEN_GL(::glDeleteShader(ID)); }

    std::uint32_t ID;
};

engine::Shader::Shader(const std::string_view vCode, const std::string_view fCode) :
    ID{::glCreateProgram()}
{
    shader_<GL_VERTEX_SHADER> vertex{vCode.data()};
    shader_<GL_FRAGMENT_SHADER> fragment{fCode.data()};

    CALL_OPEN_GL(::glAttachShader(ID, vertex.ID));
    CALL_OPEN_GL(::glAttachShader(ID, fragment.ID));
    CALL_OPEN_GL(::glLinkProgram(ID));

    GLint success;
    CALL_OPEN_GL(::glGetProgramiv(ID, GL_LINK_STATUS, &success));
    if (const auto err = glGetError(); err != GL_NO_ERROR) {
        spdlog::error("Error {} : {}", err, ::glewGetErrorString(err));
        return;
    }

    if (success == GL_FALSE) {
        GLint maxLength = 0;
        CALL_OPEN_GL(::glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &maxLength));

        std::vector<GLchar> errorLog(static_cast<std::size_t>(maxLength));
        CALL_OPEN_GL(::glGetProgramInfoLog(ID, maxLength, &maxLength, errorLog.data()));

        spdlog::error("(Failed to link shader program {}, \nError : {}\n", ID, errorLog.data());
    } else {
        spdlog::trace("Successfully created shader program {}", ID);
    }
}

engine::Shader::~Shader()
{
    CALL_OPEN_GL(::glDeleteProgram(ID));
    spdlog::trace("Destroyed shader program {}", ID);
}

auto engine::Shader::fromFile(const std::string_view vFile, const std::string_view fFile) -> Shader
{
    return {
        getFileContent(vFile).value_or(fmt::format("Cannot open file: {}", vFile)),
        getFileContent(fFile).value_or(fmt::format("Cannot open file: {}", fFile))};
}

auto engine::Shader::use() -> void { CALL_OPEN_GL(::glUseProgram(ID)); }

template<>
auto engine::Shader::setUniform(const std::string_view name, bool v) -> void
{
    if (const auto location = ::glGetUniformLocation(ID, name.data()); location != -1)
        CALL_OPEN_GL(::glUniform1ui(location, v));
}

template<>
auto engine::Shader::setUniform(const std::string_view name, float v) -> void
{
    if (const auto location = ::glGetUniformLocation(ID, name.data()); location != -1)
        CALL_OPEN_GL(::glUniform1f(location, v));
}

template<>
auto engine::Shader::setUniform(const std::string_view name, glm::mat4 mat) -> void
{
    if (const auto location = ::glGetUniformLocation(ID, name.data()); location != -1)
        CALL_OPEN_GL(::glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat)));
}
