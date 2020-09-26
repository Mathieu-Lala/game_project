#pragma once

#include <memory>
#include <fstream>

#include "Engine/Graphics.hpp"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include "Engine/Window.hpp"
#include "Engine/Shader.hpp"
#include "Engine/Utility.hpp" // for overloaded


constexpr auto VERTEX_SHADER =
R"GLSL(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}
)GLSL";

constexpr auto FRAGMENT_SHADER =
R"GLSL(#version 330 core
out vec4 FragColor;

in vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0f);
}
)GLSL";

constexpr
float VERTICES[] = {
    // positions          // colors
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // bottom left
     0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // top
};

namespace engine {

class Core {
private:

    struct hidden_type{};

public:

    struct Holder {

        std::unique_ptr<Core> &instance = Core::s_instance;

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

        ::glfwSetJoystickCallback(callback_eventJoystickConnectionDisconnection);
    }

    ~Core()
    {
        ::glfwTerminate();

        // otherwise we have a final error message at the end
        ::glfwSetErrorCallback(nullptr);

        spdlog::info("Engine::Core destroyed");
    }

    template<typename ...Args>
    auto window(Args &&...args) -> std::unique_ptr<Window> &
    {
        if (m_window) return m_window;

        m_window = std::make_unique<Window>(std::forward<Args>(args)...);
        loadOpenGL();

        return m_window;
    }

    enum EventMode {
        RECORD,
        PLAYBACK,
    };

    auto getEventMode() const { return m_eventMode; }

    auto setPendingEvents(std::vector<Event> &&events)
    {
        m_eventMode = PLAYBACK;
        m_eventsPlayback = std::move(events);
    }

    auto setPendingEventsFromFile(const char *filepath)
    {
        std::ifstream ifs(filepath);
        const auto j = nlohmann::json::parse(ifs);
        setPendingEvents(j.get<std::vector<Event>>());
    }

    auto getElapsedTime()
    {
        const auto nextTick = std::chrono::steady_clock::now();
        const auto timeElapsed = nextTick - m_lastTick;
        m_lastTick = nextTick;
        return timeElapsed;
    }

    auto getNextEvent() -> Event
    {
        ::glfwPollEvents();
        switch (m_eventMode) {
        case RECORD: {

            updateJoysticks();

            // 1. poll the core event
            if (!m_events.empty()) {
                const auto event = m_events.front();
                m_events.erase(m_events.begin());
                return event;
            }

            // 2. poll the window event
            const auto event = m_window->getNextEvent();
            if (event) return *event;

            // 3. send elapsed time
            return TimeElapsed{ getElapsedTime() };

        } break;
        case PLAYBACK: {
            if (!m_eventsPlayback.empty()) {
                auto event = m_eventsPlayback.front();
                m_eventsPlayback.erase(m_eventsPlayback.begin());

                // apply the event to the engine
                std::visit(overloaded{
                    [&](const ResizeWindow &e) { m_window->setSize({ e.width, e.height }); },
                    [&](const MoveWindow &e) { m_window->setPosition({ e.x, e.y }); },
                    [ ](const TimeElapsed &dt) { std::this_thread::sleep_for(dt.elapsed); },
                    [&](const Moved<Mouse> &m) { m_window->setCursorPosition({ m.source.x, m.source.y }); },
                    [&](const auto &e) { m_window->applyEvent(e); }
                    },
                    event);
                return event;

            } else {
                spdlog::warn("Engine::Window switching to record mode");
                m_eventMode = RECORD;

                return TimeElapsed{ getElapsedTime() };
            }

        } break;
        default: std::abort();
        }
    }

    auto main(int ac, char **av) -> int
    {
        if (!m_window)
            return 1;

        if (ac == 2) setPendingEventsFromFile(av[1]);
        spdlog::warn("Engine::Window is in {} mode", ac == 2 ? "playback" : "record");

        std::vector<Event> eventsProcessed{ TimeElapsed{} };


        Shader shader{ VERTEX_SHADER, FRAGMENT_SHADER };

        unsigned int VBO;
        unsigned int VAO;
        ::glGenVertexArrays(1, &VAO);
        ::glGenBuffers(1, &VBO);

        ::glBindVertexArray(VAO);

        ::glBindBuffer(GL_ARRAY_BUFFER, VBO);
        ::glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

        // position attribute
        ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void *>(0));
        ::glEnableVertexAttribArray(0);
        // color attribute
        ::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        ::glEnableVertexAttribArray(1);



        bool show_demo_window = true;
        while (m_window->isOpen()) {

            const auto event = getNextEvent();

            std::visit(overloaded{
                [](TimeElapsed &prev, const TimeElapsed &next) { prev.elapsed += next.elapsed; },
                [&](const auto &, const std::monostate &) {},
                // event in playback mode will be saved twice = bad !
                [&](const auto &, const auto &next) { eventsProcessed.push_back(next); } },
                eventsProcessed.back(),
                event);

            // todo : update event context with joystick/keyboard/mouse
            bool timeElapsed = false;
            bool keyPressed = false;

            // note : user defined function ? or engine related ?
            std::visit(overloaded{
                [&](const OpenWindow &) { m_lastTick = std::chrono::steady_clock::now(); },
                [&](const CloseWindow &) { m_window->close(); },
                [&](const ResizeWindow &e) { ::glViewport(0, 0, e.width, e.height); },
                [&](const TimeElapsed &) { timeElapsed = true; },
                [&](const Pressed<Key> &) { keyPressed = true; },
                [&](const Connected<Joysticks> &j) { m_joysticks.push_back(j.source); },
                [&](const Disconnected<Joysticks> &j) {
                    if (auto it = std::find_if(m_joysticks.begin(), m_joysticks.end(),
                        [id = j.source.id](auto &i){ return i.id == id; }); it != m_joysticks.end()) {

                        m_joysticks.erase(it);
                    }
                },
                [ ](const auto &) {} },
                event);

            if (keyPressed) {
                const auto keyEvent = std::get<Pressed<Key>>(event);
                if (keyEvent.source.key == GLFW_KEY_ESCAPE) // todo : abstract glfw keyboard
                    m_window->close();
            }

// note : should note draw at every frame = heavy
//            if (!timeElapsed) continue;

            m_window->draw([&] {

                if (show_demo_window)
                    ImGui::ShowDemoWindow(&show_demo_window);

                ImGui::Render();

                ::glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
                ::glClear(GL_COLOR_BUFFER_BIT);

                shader.use();
                ::glBindVertexArray(VAO);
                ::glDrawArrays(GL_TRIANGLES, 0, 3);

            });

        }


        ::glDeleteVertexArrays(1, &VAO);
        ::glDeleteBuffers(1, &VBO);

        nlohmann::json serialized(eventsProcessed);
        std::ofstream f{ "recorded_events.json" };
        f << serialized;

        return 0;
    }

private:

    static
    auto get() -> std::unique_ptr<Core> &
    {
        static auto instance = std::make_unique<Core>(hidden_type{});
        return instance;
    }

    static std::unique_ptr<Core> &s_instance;

    auto loadOpenGL()
    {
        if (const auto err = ::glewInit(); err != GLEW_OK)
            throw std::logic_error(fmt::format(
                "Engine::Core GLEW An error occured '{}' 'code={}'", ::glewGetErrorString(err), err));
    }

    // note : for now the engine support only one window
    std::unique_ptr<Window> m_window{ nullptr };

//
// Devices
//

    std::vector<Joysticks> m_joysticks;

    auto updateJoysticks() -> void
    {
        for (auto &joy : m_joysticks) {
            int count;
            auto axes = glfwGetJoystickAxes(joy.id, &count);
            for (auto i = 0; i != count; i++)
                spdlog::warn("{} => axes[{}] = {}", joy.id, i, axes[i]);

//            auto hats = glfwGetJoystickHats(joy.id, &count);
//            for (auto i = 0; i != count; i++)
//                if (hats[i])
//                    spdlog::warn("{} => hats[{}] = {}", joy.id, i, hats[i]);

            auto buttons = glfwGetJoystickButtons(joy.id, &count);
            for (auto i = 0; i != count; i++)
                if (buttons[i])
                    spdlog::warn("{} => buttons[{}] = {}", joy.id, i, buttons[i]);
        }
    }

//
// Event Handling
//

    std::chrono::steady_clock::time_point m_lastTick;

    EventMode m_eventMode{ RECORD };

    std::vector<Event> m_eventsPlayback;
    std::vector<Event> m_events;

//
// Event Callbacks
//

    static
    auto callback_eventJoystickConnectionDisconnection(int id, int event) -> void
    {
        if (s_instance->m_eventMode == RECORD) {
            if (event == GLFW_CONNECTED)
                s_instance->m_events.push_back(Connected<Joysticks>{ { id } });
            else
                s_instance->m_events.push_back(Disconnected<Joysticks>{ { id } });
        }
    }

};

} // namespace engine
