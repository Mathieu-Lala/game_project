#pragma once

#include <concepts>
#include <memory>
#include <chrono>
#include <vector>

#include <entt/entt.hpp>
#include <glm/matrix.hpp>

#include "Engine/resources/LoaderColor.hpp"
#include "Engine/resources/LoaderTexture.hpp"

namespace engine {

class Window;
class Game;
class JoystickManager;
class Shader;
class AudioManager;
struct Settings;

class Core {
private:
    struct hidden_type {
    };

public:
    // note : an Holder should NEVER be declared as global variable
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


    auto main(int argc, char **argv) -> int;


    auto getNextEvent() -> Event;

    auto setPendingEvents(std::vector<Event> &&events) -> void;

    auto setPendingEventsFromFile(const std::string_view filepath) -> bool;


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

        return m_game;
    }


    enum EventMode {
        RECORD,
        PLAYBACK,
    };

    auto close() noexcept -> void { m_is_running = false; }

    // note : not the best way ..
    auto updateView(const glm::mat4 &view) -> void;


    // note : getter

    auto window() noexcept -> std::unique_ptr<Window> & { return m_window; }

    [[nodiscard]] auto getEventMode() const noexcept { return m_eventMode; }

    [[nodiscard]] auto isRunning() const noexcept -> bool { return m_is_running; }

    template<typename T>
    auto getCache() noexcept -> entt::resource_cache<T> &;

    auto getAudioManager() noexcept -> AudioManager & { return m_audioManager; }

    auto settings() const noexcept -> Settings { return m_settings; }


private:
    static auto get() noexcept -> std::unique_ptr<Core> &;

    static Core *s_instance;

    static auto loadOpenGL() -> void;

    bool m_is_running{true};

    Settings m_settings;

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

    [[nodiscard]] auto getElapsedTime() noexcept -> std::chrono::nanoseconds;


    std::unique_ptr<JoystickManager> m_joystickManager;

    //
    // Resources
    //

    CacheColor m_colors;
    CacheTexture m_textures;

    std::unique_ptr<Shader> m_shader_colored;
    std::unique_ptr<Shader> m_shader_colored_textured;

    AudioManager m_audioManager;

#ifndef NDEBUG
    auto debugDrawJoystick() -> void;
    auto debugDrawDisplayOptions() -> void;
#endif

    std::uint32_t m_displayMode = 4; // GL_TRIANGLES
};

template<>
auto Core::getCache() noexcept -> entt::resource_cache<Color> &;

template<>
auto Core::getCache() noexcept -> entt::resource_cache<Texture> &;

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
