#pragma once

#include <optional>
#include <glm/vec2.hpp>

#include "Engine/Event/Event.hpp"

struct GLFWmonitor;
struct GLFWwindow;
struct ImGuiContext;

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


    [[nodiscard]] auto isOpen() const -> bool;

    auto close() -> void;

    auto render() -> void;

    auto setActive() -> void;

    auto setSize(glm::ivec2 &&size) -> void;

    auto setPosition(glm::ivec2 &&pos) -> void;

    auto setCursorPosition(glm::dvec2 &&pos) -> void;


    auto getAspectRatio() const -> double { return static_cast<double>(m_size.x) / static_cast<double>(m_size.y); }

    auto getSize() const -> glm::dvec2 { return m_size; }


    template<typename EventType>
    auto applyEvent([[maybe_unused]] const EventType &) -> void
    {
    }

    // note : just an utility function : may not stay
    auto draw(const std::function<void()> &drawer) -> void;

    auto getNextEvent() -> std::optional<Event>;

    auto isFullscreen() -> bool;

    auto setFullscreen(bool fullscreen) -> void;

private:
    static Window *s_instance;

    glm::ivec2 m_pos;
    glm::ivec2 m_size;

    ::GLFWmonitor *m_monitor{nullptr};
    ::GLFWwindow *m_handle{nullptr};
    ::ImGuiContext *m_ui_context{nullptr};

    //
    // Events
    //

    std::vector<Event> m_events;

    static auto callback_eventClose(GLFWwindow *window) -> void;

    static auto callback_eventResized(GLFWwindow *window, int w, int h) -> void;

    static auto callback_eventMoved(GLFWwindow *window, int x, int y) -> void;

    static auto callback_eventKeyBoard(GLFWwindow *window, int key, int scancode, int action, int mods) -> void;

    static auto callback_eventMousePressed(GLFWwindow *window, int button, int action, int mods) -> void;

    static auto callback_eventMouseMoved(GLFWwindow *window, double x, double y) -> void;
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
