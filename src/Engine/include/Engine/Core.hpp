#pragma once

#include <memory>

#include "Engine/Graphics.hpp"

#include <spdlog/spdlog.h>

#include "Engine/Window.hpp"

namespace engine {

class Core {
private:

    struct hidden_type{};

public:

    struct Holder {

        std::unique_ptr<Core> &instance = Core::get();

    };

    Core(hidden_type &&)
    {
        ::glfwSetErrorCallback([](int code, const char *message) {
            spdlog::error("Engine::Core GLFW An error occured '{}' 'code={}'\n", message, code);
        });

        spdlog::info("Engine::Core instanciated");
        if (::glfwInit() == GLFW_FALSE)
            throw std::logic_error(fmt::format("Engine::Core initialization failed"));

        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        ::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        ::glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        spdlog::info("Engine::Core GLFW version: '{}'\n", ::glfwGetVersionString());

        IMGUI_CHECKVERSION();
    }

    ~Core()
    {
        ::glfwTerminate();

        // otherwise we have a final error message at the end
        ::glfwSetErrorCallback(nullptr);

        spdlog::info("Engine::Core destroyed");
    }

    template<typename ...Args>
    [[nodiscard]]
    auto window(Args &&...args) -> std::unique_ptr<Window> &
    {
        if (m_window) return m_window;

        m_window = std::make_unique<Window>(std::forward<Args>(args)...);
        loadOpenGL();

        return m_window;
    }

private:

    [[nodiscard]]
    static
    auto get() -> std::unique_ptr<Core> &
    {
        static auto instance = std::make_unique<Core>(hidden_type{});
        return instance;
    }

    auto loadOpenGL()
    {
        if (const auto err = ::glewInit(); err != GLEW_OK)
            throw std::logic_error(fmt::format(
                "Engine::Core GLEW An error occured '{}' 'code={}'", ::glewGetErrorString(err), err));
    }

    // for now the engine support only one window
    std::unique_ptr<Window> m_window{ nullptr };

};

} // namespace engine
