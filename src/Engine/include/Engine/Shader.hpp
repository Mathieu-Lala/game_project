#pragma once

#include <string_view>
#include "Engine/details/Graphics.hpp"
#include <glm/mat4x4.hpp>

#include <spdlog/spdlog.h>

namespace engine {

class Shader {
public:
    Shader(const std::string_view vertexCode, const std::string_view fragmentCode) : ID{::glCreateProgram()}
    {
        shader_<GL_VERTEX_SHADER> vertex{vertexCode.data()};
        shader_<GL_FRAGMENT_SHADER> fragment{fragmentCode.data()};

        ::glAttachShader(ID, vertex.ID);
        ::glAttachShader(ID, fragment.ID);
        ::glLinkProgram(ID);

        GLint success;
        ::glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (auto err = glGetError() ; err != GL_NO_ERROR) {
            spdlog::error("Error {} : {}", err, glewGetErrorString(err));
            return;
        }

        if (success == GL_FALSE) {
            GLint maxLength = 0;
            ::glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> errorLog(maxLength);
            ::glGetProgramInfoLog(ID, maxLength, &maxLength, &errorLog[0]);

            spdlog::error("(Failed to link shader program {}, \nError : {}\n",
                ID,
                errorLog.data());
        } else {
            spdlog::info("Successfully created shader program {}", ID);
        }
    }

    auto static CreateFromFiles(const std::string_view vertexFile, const std::string_view fragmentFile) -> std::shared_ptr<Shader>;

    ~Shader() { ::glDeleteProgram(ID); spdlog::info("Destroyed shader program {}", ID); }

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

                std::vector<GLchar> errorLog(maxLength);
                ::glGetShaderInfoLog(ID, maxLength, &maxLength, &errorLog[0]);

                spdlog::error("(Failed to compile shader, \nError : {}\n\n{}\n)",
                    errorLog.data(),
                    source);
            }
        }

        ~shader_() { ::glDeleteShader(ID); }

        unsigned int ID;
    };
};

} // namespace engine
