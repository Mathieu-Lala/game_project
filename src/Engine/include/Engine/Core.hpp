#pragma once

#include <concepts>
#include <memory>
#include <chrono>
#include <vector>

#include <entt/entt.hpp>
#include <glm/matrix.hpp>

#include "Engine/Game.hpp"

#include "Engine/resources/LoaderColor.hpp" // note : move me in .cpp
#include "Engine/component/Texture.hpp"
#include "Engine/resources/LoaderTexture.hpp"
#include "Engine/audio/AudioManager.hpp"

namespace engine {

class Window;
class Game;
class JoystickManager;
class Shader;

class Core {
private:
    struct hidden_type {
    };

public:
    struct Holder {
        static auto init() noexcept -> Holder;

        Core *instance = Core::s_instance;
    };

    explicit Core(hidden_type &&);

    ~Core();

    Core(const Core &) = delete;
    Core(Core &&) = delete;

    Core &operator=(const Core &) = delete;
    Core &operator=(Core &&) = delete;


    auto main() -> int;

    auto getNextEvent() -> Event;

    auto window() -> std::unique_ptr<Window> & { return m_window; }

    template<typename... Args>
    auto window(Args &&... args) -> std::unique_ptr<Window> &
    {
        if (m_window != nullptr) { return m_window; }

        m_window = std::make_unique<Window>(std::forward<Args>(args)...);
        loadOpenGL();

        return m_window;
    }

    template<std::derived_from<Game> UserDefinedGame, typename... Args>
    auto game(Args &&... args) -> std::unique_ptr<Game> &
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

    [[nodiscard]] auto isRunning() const noexcept -> bool { return m_is_running; }

    auto close() noexcept -> void { m_is_running = false; }

    template<typename T>
    auto getCache() -> entt::resource_cache<T> &;

    auto getAudioManager() -> AudioManager & { return m_audioManager; }

    // note : not the best way ..
    auto updateView(const glm::mat4 &view) -> void;


private:
    static auto get() noexcept -> std::unique_ptr<Core> &;

    static Core *s_instance;

    static auto loadOpenGL() -> void;

    bool m_is_running{true};

    // note : for now the engine support only one window
    std::unique_ptr<Window> m_window{nullptr};

    //
    // World
    //

    std::unique_ptr<Game> m_game{nullptr};

    entt::registry m_world;

    //
    // Event Handling
    //

    EventMode m_eventMode{RECORD};

    std::vector<Event> m_eventsPlayback;


    std::chrono::steady_clock::time_point m_lastTick;

    auto getElapsedTime() -> std::chrono::nanoseconds;


    std::unique_ptr<JoystickManager> m_joystickManager;

    //
    // Resources
    //

    CacheColor m_colors;
    CacheTexture m_textures;

    std::unique_ptr<Shader> m_shader_colored;
    std::unique_ptr<Shader> m_shader_colored_textured;

    engine::AudioManager m_audioManager;

#ifndef NDEBUG
    auto debugDrawJoystick() -> void;
    auto debugDrawDisplayOptions() -> void;
#endif

    std::uint32_t m_displayMode = 4; // GL_TRIANGLES
};

// note : move me in .cpp

template<>
inline auto Core::getCache() -> entt::resource_cache<Color> &
{
    return m_colors;
}

// note : move me in .cpp

template<>
inline auto Core::getCache() -> entt::resource_cache<Texture> &
{
    return m_textures;
}

} // namespace engine

#ifndef NDEBUG
#    define IF_RECORD(...)                                                                                           \
        do {                                                                                                         \
            if (engine::Core::Holder{}.instance->getEventMode() == engine::Core::EventMode::RECORD) { __VA_ARGS__; } \
        } while (0)
#else
#    define IF_RECORD(...) \
        do {               \
            __VA_ARGS__;   \
        } while (0)
#endif
