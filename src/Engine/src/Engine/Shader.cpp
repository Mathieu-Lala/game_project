#include "Engine/Shader.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>
#include <fstream>


auto getFileContent(const std::string_view file)
{
    std::ifstream t(file.data());
    if (!t) throw std::runtime_error(fmt::format("Could not open file '{}'", file));

    return std::string(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>());
}


auto engine::Shader::CreateFromFiles(const std::string_view vertexFile, const std::string_view fragmentFile) -> std::shared_ptr<Shader>
{
    return std::make_shared<Shader>(getFileContent(vertexFile), getFileContent(fragmentFile));
}

auto engine::Shader::uploadUniformMat4(const std::string &name, const ::glm::mat4 &mat) -> void
{
    use();

    //
    if (!glIsProgram(ID))
        spdlog::error("{} IS NOT A PROGRAM. WTF", ID);
    //

    auto location = glGetUniformLocation(ID, name.c_str());

    if (location != -1)
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    else {
        spdlog::warn(R"(Could not find uniform '{}' in ID : {})", name, ID);

        GLint i;
        GLint count;

        GLint size;  // size of the variable
        GLenum type; // type of the variable (float, vec3 or mat4, etc)

        const GLsizei bufSize = 16; // maximum name length
        GLchar gl_name[bufSize];       // variable name in GLSL
        GLsizei length;             // name length


        glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &count);

        if (auto err = glGetError(); err != GL_NO_ERROR) {
            spdlog::error("Error {} : {}", err, glewGetErrorString(err));
            return;
        }
        spdlog::warn("Active Uniforms: {}", count);

        for (i = 0; i < count; i++) {
            glGetActiveUniform(ID, (GLuint) i, bufSize, &length, &size, &type, gl_name);

            spdlog::warn("  Uniform {} Type: {} Name: {}", i, type, gl_name);
        }
    }
}
