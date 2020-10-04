#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Shader.hpp"
#include "Engine/helpers/File.hpp"

auto engine::Shader::fromFile(const std::string_view vertexFile, const std::string_view fragmentFile) -> Shader
{
    return {getFileContent(vertexFile).value_or(""), getFileContent(fragmentFile).value_or("")};
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
            ::glGetActiveUniform(ID, static_cast<GLuint>(i), gl_name.size(), &length, &size, &type, gl_name.data());
            spdlog::warn("  Uniform {} Type: {} Name: {}", i, type, gl_name.data());
        }
    }
}
