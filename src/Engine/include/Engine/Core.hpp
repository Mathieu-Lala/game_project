#pragma once

#include <memory>
#include <entt/entt.hpp>

#include "Engine/Window.hpp"
#include "Engine/Game.hpp"
#include "Engine/JoystickManager.hpp"

namespace engine {

class Core {
private:

    struct hidden_type{};

public:

    struct Holder {

        static
        auto init() noexcept -> Holder;

        Core *instance = Core::s_instance;

    };

    explicit
    Core(hidden_type &&);

    ~Core();

    Core(const Core &) = delete;
    Core(Core &&) = delete;

    Core &operator=(const Core &) = delete;
    Core &operator=(Core &&) = delete;


    auto main() -> int;

    auto getNextEvent() -> Event;

    auto window() -> std::unique_ptr<Window> & { return m_window; }

    template<typename ...Args>
    auto window(Args &&...args) -> std::unique_ptr<Window> &
    {
        if (m_window != nullptr) { return m_window; }

        m_window = std::make_unique<Window>(std::forward<Args>(args)...);
        loadOpenGL();

        return m_window;
    }

    // note : UserDefinedGame should be a concept
    template<typename UserDefinedGame, typename ...Args>
    auto game(Args &&...args) -> std::unique_ptr<Game> &
    {
        if (m_game != nullptr) { return m_game; }

        m_game = std::make_unique<UserDefinedGame>(std::forward<Args>(args)...);
        m_game->onCreate(m_world);

        return m_game;
    }

    enum EventMode {
        RECORD,
        PLAYBACK,
    };

    auto getEventMode() const { return m_eventMode; }

    auto setPendingEvents(std::vector<Event> &&events) -> void;

    auto setPendingEventsFromFile(const std::string_view filepath) -> bool;

private:

    static
    auto get() noexcept -> std::unique_ptr<Core> &;

    static Core *s_instance;

    static
    auto loadOpenGL() -> void;

    // note : for now the engine support only one window
    std::unique_ptr<Window> m_window{ nullptr };

//
// World
//

    std::unique_ptr<Game> m_game{ nullptr };

    entt::registry m_world;

//
// Event Handling
//

    EventMode m_eventMode{ RECORD };

    // std::pmr::queue instead of vector ? try it with google benchmark
    std::vector<Event> m_eventsPlayback;


    std::chrono::steady_clock::time_point m_lastTick;

    auto getElapsedTime() -> std::chrono::nanoseconds;


    std::unique_ptr<JoystickManager> m_joystickManager;


#ifndef NDEBUG
    auto debugDrawJoystick() -> void;
#endif

};

} // namespace engine

#define IF_RECORD(...) \
    do { if (engine::Core::Holder{}.instance->getEventMode() \
        == engine::Core::EventMode::RECORD) { __VA_ARGS__; } } while(0)
