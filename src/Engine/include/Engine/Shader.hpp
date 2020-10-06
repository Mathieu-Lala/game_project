#pragma once

#include <string_view>

#include <glm/mat4x4.hpp>
#include <spdlog/spdlog.h>

#include "Engine/details/Graphics.hpp"

namespace engine {

class Shader {
public:
    Shader(const std::string_view vCode, const std::string_view fCode) : ID{::glCreateProgram()}
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

    static
    auto fromFile(const std::string_view vFile, const std::string_view fFile) -> Shader;

    ~Shader()
    {
        ::glDeleteProgram(ID);
        spdlog::info("Destroyed shader program {}", ID);
    }

    auto use() -> void { ::glUseProgram(ID); }

    auto uploadUniformMat4(const std::string &name, const ::glm::mat4 &mat) -> void;

private:
    unsigned int ID;

    // std::string debug_vertexCode;

    // helper class for consitancy
    template<std::size_t type>
    struct shader_ {
        shader_(const char *source) : ID{::glCreateShader(type)}
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

        unsigned int ID;
    };
};

} // namespace engine
