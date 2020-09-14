#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>

#include <spdlog/spdlog.h>

#include "Engine/Event.hpp"

namespace engine {

class Window {
public:

    Window(glm::ivec2 &&size, const char *title) :
        m_handle        { ::glfwCreateWindow(size.x, size.y, title, nullptr, nullptr) }, // todo : handle monitor
        m_ui_context    { ImGui::CreateContext() }
    {
        spdlog::info("Engine::Window instanciated");
        if (!m_handle)
            throw std::logic_error("Engine::Window initialization failed");

        setActive();

        if (!ImGui_ImplGlfw_InitForOpenGL(m_handle, true))
            throw std::logic_error("Engine::Window initialization glfw failed");

        if (!ImGui_ImplOpenGL3_Init("#version 130")) // todo : handle GLSL version
            throw std::logic_error("Engine::Window initialization opengl3 failed");

        // Vsync
        glfwSwapInterval(1);

        s_instance = this;

        glfwSetWindowCloseCallback(m_handle, callback_eventClose);

        // todo
//        glfwSetWindowSizeCallback(m_handle, [](GLFWwindow *, int, int){ std::cout << "resize\n"; });
//        glfwSetWindowPosCallback(m_handle, [](GLFWwindow*, int, int){ std::cout << "moved\n"; });

        glfwSetKeyCallback(m_handle, callback_eventKeyBoard);

        glfwSetMouseButtonCallback(m_handle, callback_eventMousePressed);
        glfwSetCursorPosCallback(m_handle, callback_eventMouseMoved);

        // todo : joysticks / mouse wheel / fullscreen / request_focus...
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

    Event getNextEvent()
    {
        ::glfwPollEvents();

        if (!m_events.empty()) {
            auto nextEvent = m_events.front();
            m_events.erase(m_events.begin());
            return nextEvent;

        } else {
            const auto nextTick = std::chrono::steady_clock::now();
            const auto timeElapsed = nextTick - m_lastTick;
            m_lastTick = nextTick;

            return TimeElapsed{ timeElapsed };
        }
    }

private:

    static Window *s_instance;

    std::chrono::steady_clock::time_point m_lastTick{ std::chrono::steady_clock::now() };

    ::GLFWwindow *m_handle{ nullptr };
    ::ImGuiContext *m_ui_context{ nullptr };

    std::vector<Event> m_events;

    static
    auto callback_eventClose(GLFWwindow *) -> void
    {
        s_instance->m_events.emplace_back(CloseWindow{});
    }

    static
    auto callback_eventKeyBoard(GLFWwindow *, int key, int scancode, int action, int mods) -> void
    {
        Key k{
            .alt        = !!(mods & GLFW_MOD_ALT),
            .control    = !!(mods & GLFW_MOD_CONTROL),
            .system     = !!(mods & GLFW_MOD_SUPER),
            .shift      = !!(mods & GLFW_MOD_SHIFT),
            .scancode   = scancode,
            .key        = key
        };
        switch (action) {
        case GLFW_PRESS: s_instance->m_events.emplace_back(Pressed<Key>{ std::move(k) }); break;
        case GLFW_RELEASE: s_instance->m_events.emplace_back(Released<Key>{ std::move(k) }); break;
        //case GLFW_REPEAT: s_instance->m_events.emplace_back(???{ key }); break; // todo
        default: std::abort(); break;
        }
    };

    static
    auto callback_eventMousePressed(GLFWwindow *window, int button, int action, int /* mods // todo */) -> void
    {
        double x, y;
        ::glfwGetCursorPos(window, &x, &y);
        switch (action) {
        case GLFW_PRESS: s_instance->m_events.emplace_back(Pressed<MouseButton>{ button, { x, y } }); break;
        case GLFW_RELEASE: s_instance->m_events.emplace_back(Released<MouseButton>{ button, { x, y } }); break;
        default: std::abort(); break;
        }
    }

    static
    auto callback_eventMouseMoved(GLFWwindow *, double x, double y) -> void
    {
        s_instance->m_events.emplace_back(Moved<Mouse>{ x, y });
    }

};

Window *Window::s_instance{ nullptr };

} // namespace engine
