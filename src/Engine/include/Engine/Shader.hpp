#pragma once

#include <string_view>
#include "Engine/details/Graphics.hpp"

namespace engine {

class Shader {
public:

    Shader(const std::string_view vertexCode, const std::string_view fragmentCode) :
        ID  { ::glCreateProgram() }
    {
        shader_<GL_VERTEX_SHADER> vertex{ vertexCode.data() };
        shader_<GL_FRAGMENT_SHADER> fragment{ fragmentCode.data() };

        ::glAttachShader(ID, vertex.ID);
        ::glAttachShader(ID, fragment.ID);
        ::glLinkProgram(ID);
    }

    ~Shader()
    {
        ::glDeleteProgram(ID);
    }

    auto use() -> void { ::glUseProgram(ID); }

private:
    unsigned int ID;

    // helper class for consitancy
    template<std::size_t type>
    struct shader_ {

        shader_(const char *source) :
            ID  { ::glCreateShader(type) }
        {
            ::glShaderSource(ID, 1, &source, nullptr);
            ::glCompileShader(ID);
        }

        ~shader_() { ::glDeleteShader(ID); }

        unsigned int ID;

    };

};

} // namespace engine
