#include <fstream>
#include <functional>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <magic_enum.hpp>

#include "Engine/details/overloaded.hpp"
#include "Engine/component/Drawable.hpp"

#include "Engine/Core.hpp"

engine::Core *engine::Core::s_instance{nullptr};

auto engine::Core::Holder::init() noexcept -> Holder
{
    Core::s_instance = Core::get().get();
    return {};
}

engine::Core::Core([[maybe_unused]] hidden_type &&)
{
    ::glfwSetErrorCallback([](int code, const char *message) {
        spdlog::error("Engine::Core GLFW An error occured '{}' 'code={}'\n", message, code);
    });

    spdlog::info("Engine::Core instanciated");
    if (::glfwInit() == GLFW_FALSE) { throw std::logic_error(fmt::format("Engine::Core initialization failed")); }

    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    ::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    ::glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    spdlog::info("Engine::Core GLFW version: '{}'\n", ::glfwGetVersionString());

    IMGUI_CHECKVERSION();

    m_joystickManager = std::make_unique<JoystickManager>();
}

engine::Core::~Core()
{
    ::glfwTerminate();

    // otherwise we have a final error message at the end
    ::glfwSetErrorCallback(nullptr);

    spdlog::info("Engine::Core destroyed");
}

auto engine::Core::setPendingEvents(std::vector<Event> &&events)
{
    m_eventMode = PLAYBACK;
    m_eventsPlayback = std::move(events);
}

auto engine::Core::setPendingEventsFromFile(const std::string_view filepath)
{
    std::ifstream ifs(filepath.data());
    if (!ifs.is_open()) {
        spdlog::warn("engine::Core setPendingEventsFromFile failed: {} could not be opened", filepath.data());
        return;
    }
    const auto j = nlohmann::json::parse(ifs);
    setPendingEvents(j.get<std::vector<Event>>());
}

auto engine::Core::getNextEvent() -> Event
{
    ::glfwPollEvents();

    switch (m_eventMode) {
    case RECORD: {
        m_joystickManager->poll();

        // 1. poll the window event
        // 2. poll the joysticks event
        // 3. send elapsed time
        return m_window->getNextEvent().value_or(m_joystickManager->getNextEvent().value_or(TimeElapsed{getElapsedTime()}));

    } break;
    case PLAYBACK: {
        if (m_eventsPlayback.empty()) {
            spdlog::warn("Engine::Window switching to record mode");
            m_eventMode = RECORD;
            return TimeElapsed{getElapsedTime()};
        }
        auto event = m_eventsPlayback.front();
        m_eventsPlayback.erase(m_eventsPlayback.begin());

        // apply the event to the engine
        std::visit(
            overloaded{
                [&](const ResizeWindow &e) {
                    m_window->setSize({e.width, e.height});
                },
                [&](const MoveWindow &e) {
                    m_window->setPosition({e.x, e.y});
                },
                [](const TimeElapsed &dt) { std::this_thread::sleep_for(dt.elapsed); },
                [&](const Moved<Mouse> &m) {
                    m_window->setCursorPosition({m.source.x, m.source.y});
                },
                [&](const auto &e) { m_window->applyEvent(e); },
                // todo : add fullscreen
            },
            event);
        return event;

    } break;
    default: std::abort();
    }
}

auto engine::Core::main([[maybe_unused]] const std::map<std::string, docopt::value> &args) -> int
{
    if (m_window == nullptr || m_game == nullptr) { return 1; }

#ifndef NDEBUG
    if (args.at("--play").isString()) setPendingEventsFromFile(args.at("--play").asString());

#endif
    // todo : add max size buffer ?
    std::vector<Event> eventsProcessed{TimeElapsed{}};

    while (m_window->isOpen()) { // note Core::isRunning instead ?
        const auto event = getNextEvent();

        std::visit(
            overloaded{
                [](TimeElapsed &prev, const TimeElapsed &next) { prev.elapsed += next.elapsed; },
                [&]([[maybe_unused]] const auto &, [[maybe_unused]] const std::monostate &) {},
                // event in playback mode will be saved twice = bad !
                [&]([[maybe_unused]] const auto &, const auto &next) { eventsProcessed.push_back(next); }},
            eventsProcessed.back(),
            event);

        // todo : remove me ?
        bool timeElapsed = false;
        bool keyPressed = false;

        // note : engine related
        std::visit(
            overloaded{
                [&]([[maybe_unused]] const OpenWindow &) { m_lastTick = std::chrono::steady_clock::now(); },
                [&]([[maybe_unused]] const CloseWindow &) { m_window->close(); },
                [&](const ResizeWindow &e) { ::glViewport(0, 0, e.width, e.height); },
                [&]([[maybe_unused]] const TimeElapsed &) { timeElapsed = true; },
                [&]([[maybe_unused]] const Pressed<Key> &) { keyPressed = true; },
                [&](const Connected<Joystick> &j) { m_joystickManager->add(j.source); },
                [&](const Disconnected<Joystick> &j) { m_joystickManager->remove(j.source); },
                [&](const Moved<JoystickAxis> &j) { m_joystickManager->update(j); },
                [&](const Pressed<JoystickButton> &j) { m_joystickManager->update(j); },
                [&](const Released<JoystickButton> &j) { m_joystickManager->update(j); },
                []([[maybe_unused]] const auto &) {}},
            event);

        if (keyPressed) {
            // todo : abstract glfw keyboard
            const auto keyEvent = std::get<Pressed<Key>>(event);
            if (keyEvent.source.key == GLFW_KEY_ESCAPE) m_window->close();
            if (keyEvent.source.key == GLFW_KEY_F11) m_window->setFullscreen(!m_window->isFullscreen());
        }

        m_game->onUpdate(m_world, event);

        // note : should note draw at every frame = heavy
        if (!timeElapsed) continue;

        m_window->draw([&] {
            m_game->drawUserInterface();

#ifndef NDEBUG
            debugDrawJoystick();
#endif

            ImGui::Render();

            ::glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            ::glClear(GL_COLOR_BUFFER_BIT);

            m_world.view<Drawable>().each(Drawable::system);
        });
    }

    m_world.view<engine::Drawable>().each([](engine::Drawable &drawable) {
        ::glDeleteVertexArrays(1, &drawable.VAO);
        ::glDeleteBuffers(1, &drawable.VBO);
        ::glDeleteBuffers(1, &drawable.EBO);
    });

    m_game->onDestroy(m_world);

#ifndef NDEBUG
    nlohmann::json serialized(eventsProcessed);
    std::ofstream f{"recorded_events.json"};
    f << serialized;
#endif

    // otherwise the singleton will be destroy after the main -> dead signal
    get().reset(nullptr);

    return 0;
}

auto engine::Core::get() noexcept -> std::unique_ptr<Core> &
{
    static auto instance = std::make_unique<Core>(hidden_type{});
    return instance;
}

auto engine::Core::loadOpenGL() -> void
{
    if (const auto err = ::glewInit(); err != GLEW_OK) {
        throw std::logic_error(
            fmt::format("Engine::Core GLEW An error occured '{}' 'code={}'", ::glewGetErrorString(err), err));
    }
}

auto engine::Core::getElapsedTime() -> std::chrono::nanoseconds
{
    const auto nextTick = std::chrono::steady_clock::now();
    const auto timeElapsed = nextTick - m_lastTick;
    m_lastTick = nextTick;
    return timeElapsed;
}

#ifndef NDEBUG
// todo : what happen when more than 1 joystick ?
auto engine::Core::debugDrawJoystick() -> void
{
    ImGui::Begin("Joystick");
    m_joystickManager->each([](const Joystick &joy) {
        ImGui::BeginChild("Scrolling");
        for (std::uint32_t n = 0; n < Joystick::BUTTONS_MAX; n++) {
            ImGui::Text(
                "%s = %i", magic_enum::enum_name(magic_enum::enum_cast<Joystick::Buttons>(n).value()).data(), joy.buttons[n]);
        }
        ImGui::EndChild();

        ImGui::BeginChild("Scrolling");
        for (std::uint32_t n = 0; n < Joystick::AXES_MAX; n++) {
            ImGui::Text(
                "%s = %f",
                magic_enum::enum_name(magic_enum::enum_cast<Joystick::Axis>(n).value()).data(),
                double{joy.axes[n]});
        }
        ImGui::EndChild();
    });
    ImGui::End();
}
#endif
