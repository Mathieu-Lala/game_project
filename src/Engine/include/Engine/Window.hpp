#pragma once

#include "Engine/Graphics.hpp"

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

        if (!ImGui_ImplGlfw_InitForOpenGL(m_handle, false))
            throw std::logic_error("Engine::Window initialization glfw failed");

        if (!ImGui_ImplOpenGL3_Init("#version 130")) // todo : handle GLSL version
            throw std::logic_error("Engine::Window initialization opengl3 failed");

        // Vsync
        glfwSwapInterval(1);

        s_instance = this;

        ::glfwSetWindowCloseCallback(m_handle, callback_eventClose);
        ::glfwSetWindowSizeCallback(m_handle, callback_eventResized);
        ::glfwSetWindowPosCallback(m_handle, callback_eventMoved);
        ::glfwSetKeyCallback(m_handle, callback_eventKeyBoard);
        ::glfwSetMouseButtonCallback(m_handle, callback_eventMousePressed);
        ::glfwSetCursorPosCallback(m_handle, callback_eventMouseMoved);

        // todo : joysticks / mouse wheel / fullscreen / request_focus / scroll ...
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

    auto setSize(glm::ivec2 &&size) -> void { ::glfwSetWindowSize(m_handle, size.x, size.y); }

    auto setPosition(glm::ivec2 &&pos) -> void { ::glfwSetWindowPos(m_handle, pos.x, pos.y); }

    // just an utility function : may not stay
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

    enum EventMode {
        RECORD,
        PLAYBACK,
    };

    auto getEventMode() const -> EventMode { return m_eventMode; }

    auto setPendingEvents(std::vector<Event> &&events)
    {
        m_eventMode = PLAYBACK;
        m_eventsPlayback = std::move(events);
    }

    auto getNextEvent() -> Event
    {
        ::glfwPollEvents();
        if (m_eventMode == RECORD) {

            if (m_lastTick == std::chrono::steady_clock::time_point{}) {
                m_lastTick = std::chrono::steady_clock::now();
                return OpenWindow{};
            }

            if (!m_eventsRecord.empty()) {
                auto nextEvent = m_eventsRecord.front();
                m_eventsRecord.erase(m_eventsRecord.begin());
                return nextEvent;

            } else {
                const auto nextTick = std::chrono::steady_clock::now();
                const auto timeElapsed = nextTick - m_lastTick;
                m_lastTick = nextTick;

                return TimeElapsed{ timeElapsed };
            }
        } else {
            if (!m_eventsPlayback.empty()) {
                auto event = m_eventsPlayback.front();
                m_eventsPlayback.erase(m_eventsPlayback.begin());

                std::visit(overloaded{
                    [&](const ResizeWindow &e) { this->setSize({ e.width, e.height }); },
                    [&](const MoveWindow &e) { this->setPosition({ e.x, e.y }); },
                    [](const TimeElapsed &dt) { std::this_thread::sleep_for(dt.elapsed); },
                    [&](const Moved<Mouse> &m) { ::glfwSetCursorPos(this->m_handle, m.source.x, m.source.y); },
                    [&](const Pressed<MouseButton> &m) {
                        ImGui_ImplGlfw_MouseButtonCallback(this->m_handle, m.source.button, GLFW_PRESS, 0 /* todo */);
                    },
                    [&](const Released<MouseButton> &m) {
                        ImGui_ImplGlfw_MouseButtonCallback(this->m_handle, m.source.button, GLFW_RELEASE, 0 /* todo */);
                    },
                    [&](const Pressed<Key> &k) {
                        ImGui_ImplGlfw_KeyCallback(this->m_handle, k.source.key,
                            k.source.scancode, GLFW_PRESS, 0 /* todo */);
                    },
                    [&](const Released<Key> &k) {
                        ImGui_ImplGlfw_KeyCallback(this->m_handle, k.source.key,
                            k.source.scancode, GLFW_RELEASE, 0 /* todo */);
                    },
                    [](const auto &) { }
                    },
                    event);
                return event;

            } else {
                spdlog::warn("Engine::Window switching to record mode");
                m_eventMode = RECORD;

                m_lastTick = std::chrono::steady_clock::now();
                const auto nextTick = std::chrono::steady_clock::now();
                const auto timeElapsed = nextTick - m_lastTick;

                return TimeElapsed{ timeElapsed };
            }
        }
    }

private:

    // pointer of the unique window used by callbacks
    static Window *s_instance;

    std::chrono::steady_clock::time_point m_lastTick{};

    ::GLFWwindow *m_handle{ nullptr };
    ::ImGuiContext *m_ui_context{ nullptr };

    EventMode m_eventMode{ RECORD };

    std::vector<Event> m_eventsRecord;
    std::vector<Event> m_eventsPlayback;

    static
    auto callback_eventClose(GLFWwindow *) -> void
    {
        if (s_instance->m_eventMode == RECORD) {
            s_instance->m_eventsRecord.emplace_back(CloseWindow{});
        }
    }

    static
    auto callback_eventResized(GLFWwindow *, int w, int h) -> void
    {
        if (s_instance->m_eventMode == RECORD) {
            s_instance->m_eventsRecord.emplace_back(ResizeWindow{ w, h });
        }
    }

    static
    auto callback_eventMoved(GLFWwindow *, int x, int y) -> void
    {
        if (s_instance->m_eventMode == RECORD) {
            s_instance->m_eventsRecord.emplace_back(MoveWindow{ x, y });
        }
    }

    static
    auto callback_eventKeyBoard(GLFWwindow *window, int key, int scancode, int action, int mods) -> void
    {
        if (s_instance->m_eventMode == RECORD) {
            Key k{
                .alt        = !!(mods & GLFW_MOD_ALT),
                .control    = !!(mods & GLFW_MOD_CONTROL),
                .system     = !!(mods & GLFW_MOD_SUPER),
                .shift      = !!(mods & GLFW_MOD_SHIFT),
                .scancode   = scancode,
                .key        = key
            };
            switch (action) {
            case GLFW_PRESS: s_instance->m_eventsRecord.emplace_back(Pressed<Key>{ std::move(k) }); break;
            case GLFW_RELEASE: s_instance->m_eventsRecord.emplace_back(Released<Key>{ std::move(k) }); break;
            //case GLFW_REPEAT: s_instance->m_eventsRecord.emplace_back(???{ key }); break; // todo
            default: std::abort(); break;
            }
        }
    };

    static
    auto callback_eventMousePressed(GLFWwindow *window, int button, int action, int mods) -> void
    {
        if (s_instance->m_eventMode == RECORD) {
            double x, y;
            ::glfwGetCursorPos(window, &x, &y);
            switch (action) {
            case GLFW_PRESS: s_instance->m_eventsRecord.emplace_back(Pressed<MouseButton>{ button, { x, y } }); break;
            case GLFW_RELEASE: s_instance->m_eventsRecord.emplace_back(Released<MouseButton>{ button, { x, y } }); break;
            default: std::abort(); break;
            }
        }
    }

    static
    auto callback_eventMouseMoved(GLFWwindow *, double x, double y) -> void
    {
        if (s_instance->m_eventMode == RECORD) {
            s_instance->m_eventsRecord.emplace_back(Moved<Mouse>{ x, y });
        }
    }

};

Window *Window::s_instance{ nullptr };

} // namespace engine
