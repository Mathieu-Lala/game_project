#pragma once

#include "Engine/Graphics.hpp"

namespace engine {

class Shader {
public:

    Shader(const char *vertexCode, const char *fragmentCode) :
        ID  { ::glCreateProgram() }
    {
        shader_<GL_VERTEX_SHADER> vertex{ vertexCode };
        shader_<GL_FRAGMENT_SHADER> fragment{ fragmentCode };

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
