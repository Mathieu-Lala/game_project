#include <fstream>
#include <functional>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <magic_enum.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine/details/overloaded.hpp"
#include "Engine/component/Drawable.hpp"
#include "Engine/component/Position.hpp"
#include "Engine/component/Scale.hpp"
#include "Engine/component/Velocity.hpp"

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

auto engine::Core::setPendingEvents(std::vector<Event> &&events) -> void
{
    m_eventMode = PLAYBACK;
    m_eventsPlayback = std::move(events);
}

auto engine::Core::setPendingEventsFromFile(const std::string_view filepath) -> bool
try {
    std::ifstream ifs(filepath.data());
    if (!ifs.is_open()) {
        spdlog::warn("engine::Core setPendingEventsFromFile failed: {} could not be opened", filepath.data());
        return false;
    }
    const auto j = nlohmann::json::parse(ifs);
    setPendingEvents(j.get<std::vector<Event>>());
    return true;
} catch (...) {
    spdlog::warn("engine::Core setPendingEventsFromFile failed: could not parse the file {}", filepath.data());
    return false;
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

auto engine::Core::main() -> int
{
    if (m_window == nullptr || m_game == nullptr) { return 1; }

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

        //// note : should note draw at every frame = heavy
        if (timeElapsed) {
            const auto t = std::get<TimeElapsed>(event);

            m_world.view<d2::Position, d2::Velocity>().each(
                [elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed).count()](auto &pos, auto &vel) {
                    pos.x += vel.x * static_cast<decltype(vel.x)>(elapsed) / 1000.0;
                    pos.y += vel.y * static_cast<decltype(vel.y)>(elapsed) / 1000.0;
                });

            m_window->draw([&] {
                m_game->drawUserInterface();

#ifndef NDEBUG
                debugDrawJoystick();
#endif

                enum DisplayMode {
                    POINTS = GL_POINTS,
                    LINE_STRIP = GL_LINE_STRIP,
                    LINE_LOOP = GL_LINE_LOOP,
                    LINES = GL_LINES,
                    LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY,
                    LINES_ADJACENCY = GL_LINES_ADJACENCY,
                    TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
                    TRIANGLE_FAN = GL_TRIANGLE_FAN,
                    TRIANGLES = GL_TRIANGLES,
                    TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
                    TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY,
                    PATCHES = GL_PATCHES,
                };

                static constexpr auto display_mode = std::to_array({
                    POINTS,
                    LINE_STRIP,
                    LINE_LOOP,
                    LINES,
                    LINE_STRIP_ADJACENCY,
                    LINES_ADJACENCY,
                    TRIANGLE_STRIP,
                    TRIANGLE_FAN,
                    TRIANGLES,
                    TRIANGLE_STRIP_ADJACENCY,
                    TRIANGLES_ADJACENCY,
                    PATCHES,
                });

                static auto mode = DisplayMode::TRIANGLES;
                ImGui::Begin("Display Options");
                if (ImGui::BeginCombo("##combo", "Display Mode")) {
                    for (auto n = 0ul; n < display_mode.size(); n++) {
                        bool is_selected = (mode == display_mode.at(n));
                        if (ImGui::Selectable(magic_enum::enum_name<DisplayMode>(display_mode.at(n)).data(), is_selected))
                            mode = display_mode.at(n);
                        if (is_selected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::End();

                ImGui::Render();

                // todo : add Game::getBackgroundColor()
                ::glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
                ::glClear(GL_COLOR_BUFFER_BIT);

                m_world.view<Drawable, d2::Position, d2::Scale>().each([](auto &drawable, auto &pos, auto &scale) {
                    drawable.shader->use();
                    auto model = glm::dmat4(1.0);
                    model = glm::scale(model, glm::dvec3{scale.x, scale.y, 0.0});
                    model = glm::translate(model, glm::dvec3{pos.x, pos.y, 0.0});
                    //              model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
                    drawable.shader->uploadUniformMat4("model", model);

                    ::glBindVertexArray(drawable.VAO);
                    ::glDrawElements(static_cast<std::uint32_t>(mode), 3 * drawable.triangle_count, GL_UNSIGNED_INT, 0);
                });
            });
        }

        m_game->onUpdate(m_world, event);
    }

    // todo : use entt::on_destroy or something ..
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
