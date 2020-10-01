#include <spdlog/spdlog.h>

#include "Engine/Window.hpp"
#include "Engine/Core.hpp"

engine::Window *engine::Window::s_instance{nullptr};

engine::Window::Window(glm::ivec2 &&size, const char *title) :
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    m_handle{::glfwCreateWindow(size.x, size.y, title, nullptr, nullptr)}, // todo : handle monitor
    m_ui_context{ImGui::CreateContext()}
{
    spdlog::info("Engine::Window instanciated");
    if (m_handle == nullptr) { throw std::logic_error("Engine::Window initialization failed"); }

    setActive();

    if (!ImGui_ImplGlfw_InitForOpenGL(m_handle, false)) {
        throw std::logic_error("Engine::Window initialization glfw failed");
    }

    if (!ImGui_ImplOpenGL3_Init()) { throw std::logic_error("Engine::Window initialization opengl3 failed"); }

    // Vsync
    ::glfwSwapInterval(1);

    s_instance = this;

    ::glfwSetWindowCloseCallback(m_handle, callback_eventClose);
    ::glfwSetWindowSizeCallback(m_handle, callback_eventResized);
    ::glfwSetWindowPosCallback(m_handle, callback_eventMoved);
    ::glfwSetKeyCallback(m_handle, callback_eventKeyBoard);
    ::glfwSetMouseButtonCallback(m_handle, callback_eventMousePressed);
    ::glfwSetCursorPosCallback(m_handle, callback_eventMouseMoved);

    // todo : mouse wheel / fullscreen / request_focus ...

    m_events.emplace_back(OpenWindow{});
}

engine::Window::~Window() noexcept(true)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext(m_ui_context);

    if (m_handle != nullptr) { ::glfwDestroyWindow(m_handle); }

    spdlog::info("Engine::Window destroyed");
}

auto engine::Window::getNextEvent() -> std::optional<Event>
{
    if (m_events.empty()) { return {}; }

    auto nextEvent = m_events.front();
    m_events.erase(m_events.begin());
    return nextEvent;
}

auto engine::Window::callback_eventClose([[maybe_unused]] GLFWwindow *window) -> void
{
    if (Core::Holder{}.instance->getEventMode() == Core::EventMode::RECORD) {
        s_instance->m_events.emplace_back(CloseWindow{});
    }
}

auto engine::Window::callback_eventResized([[maybe_unused]] GLFWwindow *window, int w, int h) -> void
{
    if (Core::Holder{}.instance->getEventMode() == Core::EventMode::RECORD) {
        s_instance->m_events.emplace_back(ResizeWindow{w, h});
    }
}

auto engine::Window::callback_eventMoved([[maybe_unused]] GLFWwindow *window, int x, int y) -> void
{
    if (Core::Holder{}.instance->getEventMode() == Core::EventMode::RECORD) {
        s_instance->m_events.emplace_back(MoveWindow{x, y});
    }
}

auto engine::Window::callback_eventKeyBoard([[maybe_unused]] GLFWwindow *window, int key, int scancode, int action, int mods)
    -> void
{
    if (Core::Holder{}.instance->getEventMode() == Core::EventMode::RECORD) {
        // clang-format off
        Key k{
            .alt        = !!(mods & GLFW_MOD_ALT), // NOLINT
            .control    = !!(mods & GLFW_MOD_CONTROL), // NOLINT
            .system     = !!(mods & GLFW_MOD_SUPER), // NOLINT
            .shift      = !!(mods & GLFW_MOD_SHIFT), // NOLINT
            .scancode   = scancode,
            .key        = key
        };
        // clang-format on
        switch (action) {
        case GLFW_PRESS: s_instance->m_events.emplace_back(Pressed<Key>{k}); break;
        case GLFW_RELEASE: s_instance->m_events.emplace_back(Released<Key>{k}); break;
        // case GLFW_REPEAT: s_instance->m_events.emplace_back(???{ key }); break; // todo
        default: std::abort(); break;
        }
    }
}

auto engine::Window::callback_eventMousePressed(GLFWwindow *window, int button, int action, [[maybe_unused]] int mods) -> void
{
    if (Core::Holder{}.instance->getEventMode() == Core::EventMode::RECORD) {
        double x = 0;
        double y = 0;
        ::glfwGetCursorPos(window, &x, &y);
        switch (action) {
        case GLFW_PRESS: s_instance->m_events.emplace_back(Pressed<MouseButton>{button, {x, y}}); break;
        case GLFW_RELEASE: s_instance->m_events.emplace_back(Released<MouseButton>{button, {x, y}}); break;
        default: std::abort(); break;
        }
    }
}

auto engine::Window::callback_eventMouseMoved([[maybe_unused]] GLFWwindow *window, double x, double y) -> void
{
    if (Core::Holder{}.instance->getEventMode() == Core::EventMode::RECORD) {
        s_instance->m_events.emplace_back(Moved<Mouse>{x, y});
    }
}

using namespace engine; // just for the template specialization consistency

template<>
auto Window::applyEvent<Pressed<MouseButton>>(const Pressed<MouseButton> &m) -> void
{
    ImGui_ImplGlfw_MouseButtonCallback(m_handle, m.source.button, GLFW_PRESS, 0 /* todo */);
}

template<>
auto Window::applyEvent<Released<MouseButton>>(const Released<MouseButton> &m) -> void
{
    ImGui_ImplGlfw_MouseButtonCallback(m_handle, m.source.button, GLFW_RELEASE, 0 /* todo */);
}

template<>
auto Window::applyEvent<Pressed<Key>>(const Pressed<Key> &k) -> void
{
    ImGui_ImplGlfw_KeyCallback(m_handle, k.source.key, k.source.scancode, GLFW_PRESS, 0 /* todo */);
}

template<>
auto Window::applyEvent<Released<Key>>(const Released<Key> &k) -> void
{
    ImGui_ImplGlfw_KeyCallback(m_handle, k.source.key, k.source.scancode, GLFW_RELEASE, 0 /* todo */);
}