#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>

#include <spdlog/spdlog.h>

#include <iostream>

namespace engine {

class Window {
public:

    Window(glm::ivec2 &&size, const char *title) :
        m_handle   { ::glfwCreateWindow(size.x, size.y, title, nullptr, nullptr) }
    {
        spdlog::info("Engine::Window instanciated");
        if (!m_handle)
            throw std::logic_error("Engine::Window initialization failed");

        setActive();

        m_ui_context = ImGui::CreateContext();

        if (!ImGui_ImplGlfw_InitForOpenGL(m_handle, true))
            throw int{};

        if (!ImGui_ImplOpenGL3_Init("#version 130")) // GLSL version
            throw int{};

        // Vsync
        glfwSwapInterval(1);

        // todo
        glfwSetWindowCloseCallback(m_handle, [](GLFWwindow *window){ std::cout << "closing\n"; });
        glfwSetWindowSizeCallback(m_handle, [](GLFWwindow *, int, int){ std::cout << "resize\n"; });
        glfwSetWindowPosCallback(m_handle, [](GLFWwindow*, int, int){ std::cout << "moved\n"; });
        glfwSetKeyCallback(m_handle, [](GLFWwindow *, int, int, int, int){ std::cout << "key pressed\n"; });
        glfwSetMouseButtonCallback(m_handle, [](GLFWwindow *, int, int, int){ std::cout << "mouse pressed\n"; });
        glfwSetCursorPosCallback(m_handle, [](GLFWwindow *, double, double){ std::cout << "mouse moved\n"; });
        // missing joysticks
    }

    ~Window()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext(m_ui_context);

        if (m_handle)
            ::glfwDestroyWindow(m_handle);

        spdlog::info("Engine::Window destroyed");
    }

    auto isOpen() const -> bool { return !::glfwWindowShouldClose(m_handle); }

    auto close() -> void { ::glfwSetWindowShouldClose(m_handle, true); }

    auto render() -> void { ::glfwSwapBuffers(m_handle); }

    auto setActive() -> void { ::glfwMakeContextCurrent(m_handle); }

    auto draw(const std::function<void()> &drawer)
    {
        setActive();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        drawer();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        render();
    }

//private:

    ::GLFWwindow *m_handle{ nullptr };
    ::ImGuiContext *m_ui_context{ nullptr };

};

} // namespace engine
