#pragma once

#include <GL/glew.h>

namespace engine {

class Shader {
public:

    Shader(const char *vertexCode, const char *fragmentCode)
    {
        auto vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexCode, NULL);
        glCompileShader(vertex);

        auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentCode, NULL);
        glCompileShader(fragment);

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    ~Shader()
    {
        glDeleteProgram(ID);
    }

    auto use() -> void { glUseProgram(ID); }

private:
    unsigned int ID;

};

} // namespace engine
