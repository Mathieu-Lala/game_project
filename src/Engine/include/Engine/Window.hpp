#pragma once

#include <optional>
#include <glm/vec2.hpp>

#include "Engine/details/Graphics.hpp"

#include "Engine/Event.hpp"

namespace engine {

class Window {
public:

    enum Property {
        DEFAULT,
        FULLSCREEN,

        PROPERTY_MAX
    };

    Window(glm::ivec2 &&size, const std::string_view title, std::uint16_t property = DEFAULT);
    ~Window();


    [[nodiscard]] auto isOpen() const -> bool { return ::glfwWindowShouldClose(m_handle) == GLFW_FALSE; }

    auto close() { ::glfwSetWindowShouldClose(m_handle, GLFW_TRUE); }

    auto render() { ::glfwSwapBuffers(m_handle); }

    auto setActive() -> void { ::glfwMakeContextCurrent(m_handle); }

    auto setSize(glm::ivec2 &&size) { ::glfwSetWindowSize(m_handle, size.x, size.y); }

    auto setPosition(glm::ivec2 &&pos) { ::glfwSetWindowPos(m_handle, pos.x, pos.y); }

    auto setCursorPosition(glm::dvec2 &&pos) { ::glfwSetCursorPos(m_handle, pos.x, pos.y); }


    auto getAspectRatio() const -> double { return static_cast<double>(m_size.x) / static_cast<double>(m_size.y); }


    template<typename EventType>
    auto applyEvent([[maybe_unused]] const EventType &) -> void { }

    // just an utility function : may not stay
    auto draw(const std::function<void()> &drawer)
    {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        drawer();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        render();
    }

    auto getNextEvent() -> std::optional<Event>;

    auto isFullscreen() -> bool;

    auto setFullscreen(bool fullscreen) -> void;

private:

    static Window *s_instance;

    glm::ivec2 m_pos;
    glm::ivec2 m_size;

    ::GLFWmonitor *m_monitor{ nullptr };
    ::GLFWwindow *m_handle{ nullptr };
    ::ImGuiContext *m_ui_context{ nullptr };

//
// Events
//

    std::vector<Event> m_events;

    static
    auto callback_eventClose(GLFWwindow *window) -> void;

    static
    auto callback_eventResized(GLFWwindow *window, int w, int h) -> void;

    static
    auto callback_eventMoved(GLFWwindow *window, int x, int y) -> void;

    static
    auto callback_eventKeyBoard(GLFWwindow *window, int key, int scancode, int action, int mods) -> void;

    static
    auto callback_eventMousePressed(GLFWwindow *window, int button, int action, int mods) -> void;

    static
    auto callback_eventMouseMoved(GLFWwindow *window, double x, double y) -> void;

};

// defined in Window.cpp

template<>
auto Window::applyEvent(const Pressed<MouseButton> &) -> void;

template<>
auto Window::applyEvent(const Released<MouseButton> &) -> void;

template<>
auto Window::applyEvent(const Pressed<Key> &) -> void;

template<>
auto Window::applyEvent(const Released<Key> &) -> void;

} // namespace engine
